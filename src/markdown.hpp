#include <filesystem>
#include <string>

struct Post {
  std::string title;
  std::string date;
  std::string md;   // raw markdown
  std::string html; // processed html
  std::filesystem::path src_pth;
  std::filesystem::path out_pth;
  bool has_math;          // requires MathJax
  bool has_twitter_embed; // has twitter embeds
};

struct Config {
  std::filesystem::path content_dir;
  std::filesystem::path output_dir;
  std::filesystem::path root_dir;
};

// core markdown processing functions
namespace md {
// main processing functions
std::string to_html(const std::string& md);
Post process_file(const std::filesystem::path& pth, const Config& config);

// block processing
std::string process_headers(const std::string& block);
std::string process_math(const std::string& block);
std::string process_blockquote(const std::string& block);

// inline processing
std::string process_emphasis(const std::string& text);
std::string process_links(const std::string& text);
std::string process_images(const std::string& text);

// special elements
std::string process_latex(const std::string& text);
std::string process_twitter_embed(const std::string& text);

// html generation
std::string generate_html(const Post& post);

// file operations
std::vector<std::filesystem::path> find_md_files(const Config& config);
void write_post(const Post& post);
void write_index(const std::vector<Post>& posts, const Config& config);

} // namespace md
