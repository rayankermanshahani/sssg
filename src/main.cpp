#include "markdown.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

void print_usage() {
  std::cout << "Usage: sssg [OPTIONS]\n"
            << "\nDescription:\n"
            << "  sssg is a simple static site generator that converts "
               "markdown files into consistently\n"
            << "  formatted HTML files\n"
            << "\nOptions:\n"
            << "      --root DIR        Website root directory (default: .)\n"
            << "  -i, --input DIR       Input directory for markdown files\n"
            << "  -o, --output DIR      Output directory for generated HTML\n"
            << "  -h, --help            Show this help message and exit\n"
            << std::endl;
}
Config parse_args(int argc, char* argv[]) {
  Config config;
  // root directory defaults to current working directory
  config.root_dir = std::filesystem::current_path();
  std::string content_dir;
  std::string output_dir;

  if (argc == 1) { // not enough arguments
    throw std::runtime_error("Missing required --input and --output arguments");
  }

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "--help") {
      print_usage();
      std::exit(0);
    } else if (arg == "--root") {
      if (i + 1 < argc) {
        config.root_dir = argv[++i];
      } else {
        throw std::runtime_error("--root option requires a directory path");
      }
    } else if (arg == "--input" || arg == "-i") {
      if (i + 1 < argc) {
        content_dir = argv[++i];
      } else {
        throw std::runtime_error("--input option requires a directory path");
      }
    } else if (arg == "--output" || arg == "-o") {
      if (i + 1 < argc) {
        output_dir = argv[++i];
      } else {
        throw std::runtime_error("--output option requires a directory path");
      }
    } else {
      throw std::runtime_error("Unknown option: " + arg);
    }
  }

  // input and output directories must be specified
  if (content_dir.empty()) {
    throw std::runtime_error("Missing required --input argument");
  }
  if (output_dir.empty()) {
    throw std::runtime_error("Missing required --output argument");
  }

  // set up input and output subdirectories relative to root
  config.content_dir = config.root_dir / content_dir;
  config.output_dir = config.root_dir / output_dir;

  // verify directory structure
  if (!std::filesystem::exists(config.root_dir)) {
    throw std::runtime_error("Website root directory does not exist: " +
                             config.root_dir.string());
  }

  if (!std::filesystem::exists(config.content_dir)) {
    throw std::runtime_error("Input directory does not exist: " +
                             config.content_dir.string());
  }
  if (!std::filesystem::exists(config.root_dir / "styles")) {
    throw std::runtime_error("Styles directory does not exist: " +
                             (config.root_dir / "styles").string());
  }

  // create output directory if it does not exist
  std::filesystem::create_directories(config.output_dir);

  return config;
}

int main(int argc, char* argv[]) {
  try {
    // parse command line arguments
    Config config = parse_args(argc, argv);
    std::cout << "Website root: " << config.root_dir << "\n"
              << "Processing markdown files from: " << config.content_dir
              << "\n"
              << "Generating HTML files to: " << config.output_dir << "\n";

    // find all markdown files
    auto md_files = md::find_md_files(config);
    if (md_files.empty()) {
      std::cout << "No markdown files found in " << config.content_dir << "\n";
      return 0;
    }

    // process each markdown file
    std::vector<Post> posts;
    for (const auto& file : md_files) {
      std::cout << "Processing: " << file.filename() << "\n";
      Post post = md::process_file(file, config);
      md::write_post(post);
      posts.push_back(post);
    }

    // generate index page
    std::cout << "Generating writing index page\n";
    md::write_index(posts, config);
    std::cout << "Site generation complete.\n"
              << "Processed " << posts.size() << " posts\n";
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n\n";
    print_usage();
    return 1;
  }

  return 0;
}
