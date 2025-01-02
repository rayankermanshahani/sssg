#include "markdown.hpp"
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>

namespace md {
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
  if (first_ln.find("# ") == 0) {
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

// TODO: find good c++17 equivalent of std::string::starts_with() and
// std::string::ends_with()
std::string process_math(const std::string& block) {
  std::string result = block;
  return result;
}
std::string process_blockquote(const std::string& block);

// inline processing
std::string process_emphasis(const std::string& text);
std::string process_links(const std::string& text);
std::string process_images(const std::string& text);

// special elements
std::string process_latex(const std::string& text);
std::string process_twitter_embed(const std::string& text);

// template handling
std::string apply_template(const std::string& content, const Post& post);

// file operations
std::vector<std::filesystem::path> find_md_files(const Config& config);
void write_post(const Post& post, const Config& config);
void write_index(const std::vector<Post>& posts, const Config& config);

} // namespace md
