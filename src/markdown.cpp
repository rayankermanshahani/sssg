#include "markdown.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

namespace md {

static bool starts_with(const std::string& str, const std::string& prefix) {
  return str.size() >= prefix.size() &&
         str.compare(0, prefix.size(), prefix) == 0;
}

// process markdown to html content
std::string to_html(const std::string& md) {
  std::string html = md;

  // split into blocks (double newline)
  std::istringstream stream(md);
  std::string line;
  std::string result;
  std::string current_block;

  while (std::getline(stream, line)) {
    if (line.empty()) {
      if (!current_block.empty()) {
        // process accumulated block
        std::string processed = current_block;
        processed = process_headers(processed);
        processed = process_math(processed);
        processed = process_blockquote(processed);
        processed = process_emphasis(processed);
        processed = process_images(processed);
        processed = process_links(processed);
        processed = process_twitter_embed(processed);

        result += "<p>" + processed + "</p>\n" + "\n<br>\n<br>\n";
        current_block.clear();
      }
      continue;
    }
    if (current_block.empty()) {
      current_block = line;
    } else {
      current_block += "\n" + line;
    }
  }

  // process any remaining blocks
  if (!current_block.empty()) {
    std::string processed = current_block;
    processed = process_headers(processed);
    processed = process_math(processed);
    processed = process_blockquote(processed);
    processed = process_emphasis(processed);
    processed = process_images(processed);
    processed = process_links(processed);
    processed = process_twitter_embed(processed);

    result += processed;
  }

  return result;
}

Post process_file(const std::filesystem::path& pth, const Config& config) {
  Post post;
  post.src_pth = pth;

  // read file content
  std::ifstream file(pth);
  std::stringstream buffer;
  buffer << file.rdbuf();
  post.md = buffer.str();

  // extract title (h1) from first line
  std::string first_ln = post.md.substr(0, post.md.find('\n'));
  if (starts_with(first_ln, "# ")) {
    post.title = first_ln.substr(2);

    // remove that line from the markdown, so it doesn't show up again as a
    // header
    std::size_t pos = post.md.find('\n');
    if (pos != std::string::npos) {
      // this chops off the first line plus the newline
      post.md = post.md.substr(pos + 1);
    }
  }

  // extract date from filename (assuming YYYY-MM-DD-title.md)
  std::string filename = pth.stem().string();
  if (filename.length() >= 10) {
    post.date = filename.substr(0, 10);
    // convert YYYY-MM-DD to YYYY/MM/DD format
    std::replace(post.date.begin(), post.date.end(), '-', '/');
  }

  // process md to html
  post.html = to_html(post.md);

  // check for special features
  post.has_math = post.md.find("$$") != std::string::npos ||
                  post.md.find("$") != std::string::npos;
  post.has_twitter_embed = post.md.find("<tweet>") != std::string::npos;

  // set output path
  post.out_pth = config.output_dir / pth.stem().concat(".html");

  return post;
}

// block processing
std::string process_headers(const std::string& block) {
  std::string result = block;

  std::regex h1_pattern("^# (.+)$");
  std::regex h2_pattern("^## (.+)$");
  std::regex h3_pattern("^### (.+)$");

  result = std::regex_replace(result, h1_pattern, "<h1>$1</h1>");
  result = std::regex_replace(result, h2_pattern, "<h2>$1</h2>");
  result = std::regex_replace(result, h3_pattern, "<h3>$1</h3>");

  return result;
}

std::string process_math(const std::string& text) {
  std::string result = text;

  // process multiline math blocks
  static const std::regex block_pattern(R"(\${2}([\s\S]+?)\${2})");
  result = std::regex_replace(result, block_pattern, "\\[$1\\]");

  // process inline math
  static const std::regex inline_pattern(R"(\$([^\$]+?)\$)");
  result = std::regex_replace(result, inline_pattern, "\\($1\\)");

  return result;
}

std::string process_blockquote(const std::string& block) {
  if (block.empty() && block[0] == '>') {
    std::string content = block.substr(1);
    return "<blockquote>" + content + "</blockquote>";
  }
  return block;
}

// inline processing
std::string process_emphasis(const std::string& text) {
  std::string result = text;

  // bold
  std::regex bold_pattern("\\*\\*(.+?)\\*\\*");
  result = std::regex_replace(result, bold_pattern, "<strong>$1</strong>");

  // italics
  std::regex italic_pattern("\\*(.+?)\\*");
  result = std::regex_replace(result, italic_pattern, "<em>$1</em>");

  return result;
}

std::string process_links(const std::string& text) {
  std::regex link_pattern("\\[(.+?)\\]\\((.+?)\\)");
  return std::regex_replace(text, link_pattern, "<a href=\"$2\">$1</a>");
}

std::string process_images(const std::string& text) {
  std::regex img_pattern("!\\[([^\\]]*)\\]\\(([^\\)]*)\\)");
  return std::regex_replace(text, img_pattern, "<img src=\"$2\" alt=\"$1\">");
}

// special elements
std::string process_twitter_embed(const std::string& text) {
  std::regex tweet_pattern("<tweet>(.+?)</tweet>");
  return std::regex_replace(
      text, tweet_pattern,
      "<blockquote class=\"twitter-tweet\" data-media-max-width=\"560\"><a "
      "href=\"https://twitter.com/x/status/$1\"></a></blockquote>\n<script "
      "async src=\"https://platform.twitter.com/widgets.js\" "
      "charset=\"utf-8\"></script>");
}

// generate html
std::string generate_html(const Post& post) {
  std::stringstream html;
  html << "<!DOCTYPE html>\n"
       << "<html>\n"
       << "<head>\n"
       << "    <title>" << post.title << "</title>\n"
       << "    <meta charset=\"utf-8\">\n"
       << "    <meta name=\"viewport\" content=\"width=device-width, "
          "initial-scale=1.0\">\n"
       << "    <link rel=\"stylesheet\" type=\"text/css\" "
          "href=\"../styles/reset.css\" />\n"
       << "    <link rel=\"stylesheet\" type=\"text/css\" "
          "href=\"../styles/writingstyle.css\" />\n";

  if (post.has_math) {
    html << "    <script "
            "src=\"https://cdn.mathjax.org/mathjax/latest/"
            "MathJax.js?config=TeX-AMS-MML_HTMLorMML\"></script>\n";
  }

  html << "</head>\n"
       << "<body>\n"
       << "    <div id=\"contents\">\n"
       << "        <br>\n"
       << "        <br>\n"
       << "        <br>\n"
       << "        <div id=\"date\">" << post.date << "</div>\n"
       << "        <br>\n"
       << "        <br>\n"
       << "        <div id=\"title\">" << post.title << "</div>\n"
       << "        <br>\n"
       << "        <hr>\n"
       << "        <br>\n"
       << "        <br>\n"
       << post.html << "        <br>\n"
       << "        <br>\n"
       << "        <hr>\n"
       << "        <br>\n"
       << "        <br>\n"
       << "        <a href=\"../../writing.html\" id=\"back-link\"> &lt; "
          "return </a>\n"
       << "    </div>\n"
       << "</body>\n"
       << "</html>";

  return html.str();
}

// file operations
std::vector<std::filesystem::path> find_md_files(const Config& config) {
  std::vector<std::filesystem::path> md_files;

  for (const auto& entry :
       std::filesystem::directory_iterator(config.content_dir)) {
    if (entry.path().extension() == ".md") {
      md_files.push_back(entry.path());
    }
  }

  // sort by filename (containing date) in reverse order
  std::sort(md_files.begin(), md_files.end(),
            std::greater<std::filesystem::path>());

  return md_files;
}

void write_post(const Post& post) {
  std::filesystem::create_directories(post.out_pth.parent_path());
  std::ofstream out_file(post.out_pth);
  out_file << generate_html(post);
}

void write_index(const std::vector<Post>& posts, const Config& config) {
  std::stringstream html;

  html << "<!DOCTYPE html>\n"
       << "<html>\n"
       << "<head>\n"
       << "    <title>Writing</title>\n"
       << "    <meta charset=\"utf-8\">\n"
       << "    <meta name=\"viewport\" content=\"width=device-width, "
          "initial-scale=1.0\">\n"
       << "    <link rel=\"stylesheet\" type=\"text/css\" "
          "href=\"./styles/reset.css\" />\n"
       << "    <link rel=\"stylesheet\" type=\"text/css\" "
          "href=\"./styles/stylesheet.css\" />\n"
       << "    <style>\n"
       << "      p, ul {\n"
       << "        text-align: left;\n"
       << "      }\n"
       << "    </style>\n"
       << "</head>\n\n"
       << "<body>\n"
       << "    <div id=\"contents\">\n"
       << "        <ul>\n"
       << "        <p>Posts:</p>\n";

  for (const auto& post : posts) {
    html << "            <p><a href=\"writing/"
         << post.out_pth.filename().string() << "\"><u>" << post.title
         << "</u></a></p>\n";
  }

  html << "          <p><a href=\"./index.html\">&lt; return</a></p>"
       << "        </ul>\n"
       << "    </div>\n"
       << "</body>\n"
       << "</html>";

  std::ofstream index_file(config.root_dir / "writing.html");
  index_file << html.str();
}

} // namespace md
