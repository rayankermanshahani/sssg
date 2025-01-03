#include "markdown.hpp"
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <string_view>

namespace md {

static bool starts_with(const std::string& str, const std::string& prefix) {
  return str.size() >= prefix.size() &&
         str.compare(0, prefix.size(), prefix) == 0;
}

static bool ends_with(const std::string& str, const std::string& suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// process markdown to html content
std::string to_html(const std::string& md) {
  std::string html = md;

  // split into blocks (double newline)
  std::istringstream stream(md);
  std::string block;
  std::string result;
  std::string current_block;

  while (std::getline(stream, block)) {
    if (block.empty()) {
      if (!current_block.empty()) {
        // process accumulated block
        result += process_headers(current_block);
        result += process_math(current_block);
        result += process_blockquote(current_block);
        current_block.clear();
      }
      continue;
    }
    current_block += block + "\n";
  }

  // process any remaining blocks
  if (!current_block.empty()) {
    result += process_headers(current_block);
    result += process_math(current_block);
    result += process_blockquote(current_block);
  }

  // process inline elements
  result = process_emphasis(result);
  result = process_links(result);
  result = process_images(result);
  result = process_latex(result);
  result = process_twitter_embed(result);

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
  }

  // extract date from filename (assuming YYYY-MM-DD-title.md)
  std::string filename = pth.stem().string();
  if (filename.length() >= 10) {
    post.date = filename.substr(0, 10);
  }

  // process md to html
  post.html = to_html(post.md);

  // check for special features
  post.has_math = post.md.find("$$") != std::string::npos;
  post.has_twitter_embed = post.md.find("<tweet>") != std::string::npos;

  // set output path
  post.out_pth = config.output_dir / pth.stem().concat("html");

  return post;
}

// block processing
std::string process_headers(const std::string& block) {
  std::regex h1_pattern("^# (.+)$");
  std::regex h2_pattern("^## (.+)$");
  std::regex h3_pattern("^### (.+)$");

  std::string result = block;
  result = std::regex_replace(result, h1_pattern, "<h1>$1</h1>");
  result = std::regex_replace(result, h2_pattern, "<h2>$1</h2>");
  result = std::regex_replace(result, h3_pattern, "<h3>$1</h3>");

  return result;
}

std::string process_math(const std::string& block) {
  if (block.size() >= 4 && starts_with(block, "$$") && ends_with(block, "$$")) {
    std::string math = block.substr(2, block.length() - 4);
    return "<div class=\"math\">\\[" + math + "\\]</div>";
  }
  return block;
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
  std::regex img_pattern("!\\[(.+?)\\]\\((.+?)\\)");
  return std::regex_replace(text, img_pattern, "<img src=\"$2\" alt=\"$1\">");
}

// special elements
std::string process_latex(const std::string& text) {
  std::regex inline_math_pattern("\\$(.+?)\\$");
  return std::regex_replace(text, inline_math_pattern,
                            "<span class=\"math\">\\\\($1\\\\)</span>");
}
std::string process_twitter_embed(const std::string& text) {
  std::regex tweet_pattern("<tweet>(.+?)</tweet>");
  return std::regex_replace(
      text, tweet_pattern,
      "<blockquote class=\"twitter-tweet\"><a "
      "href=\"https://twitter.com/x/status/$1\"></a></blockquote>");
}

// template handling
std::string apply_template(const std::string& content, const Post& post) {
  // read template file
  std::ifstream template_file("template.html");
  std::string templ((std::istreambuf_iterator<char>(template_file)),
                    std::istreambuf_iterator<char>());

  // replace placeholders
  std::regex title_pattern("\\{\\{title\\}\\}");
  std::regex date_pattern("\\{\\{date\\}\\}");
  std::regex content_pattern("\\{\\{content\\}\\}");

  std::string result = templ;
  result = std::regex_replace(result, title_pattern, post.title);
  result = std::regex_replace(result, date_pattern, post.date);
  result = std::regex_replace(result, content_pattern, content);

  return result;
}

// TODO: COMPLETE FILE OPS
// file operations
std::vector<std::filesystem::path> find_md_files(const Config& config);
void write_post(const Post& post, const Config& config);
void write_index(const std::vector<Post>& posts, const Config& config);

} // namespace md
