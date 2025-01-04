#include "markdown.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

void print_usage() {
  std::cout << "Usage: sssg [OPTIONS]\n"
            << "Options:\n"
            << "  -i, --input DIR    Input directory containing markdown files "
               "(default: content/)\n"
            << "  -o, --output DIR   Output directory for generated HTML "
               "(default: writing/)\n"
            << "  -h, --help         Show this help message\n";
}

Config parse_args(int argc, char* argv[]) {
  Config config;
  config.content_dir = "content/"; // default input directory
  config.output_dir = "writing/";  // default output directory
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[1];

    if (arg == "--help") {
      print_usage();
      std::exit(0);
    } else if (arg == "--input" || arg == "-i") {
      if (i + 1 < argc) {
        config.content_dir = argv[++i];
      } else {
        throw std::runtime_error("--input option requires a directory path");
      }
    } else if (arg == "--output" || arg == "-o") {
      if (i + 1 < argc) {
        config.content_dir = argv[++i];
      } else {
        throw std::runtime_error("--output option requires a directory path");
      }
    } else {
      throw std::runtime_error("Unknown option: " + arg);
    }
  }

  // convert to absolute paths and ensure directories exist
  config.content_dir = std::filesystem::absolute(config.content_dir);
  config.output_dir = std::filesystem::absolute(config.output_dir);

  if (!std::filesystem::exists(config.content_dir)) {
    throw std::runtime_error("Input directory does not exist: " +
                             config.content_dir.string());
  }

  // create output directory if it does not exist
  std::filesystem::create_directories(config.output_dir);

  return config;
}

void copy_assets(const Config& config) {
  // check if assets directory exists in content directory
  std::filesystem::path assets_src = config.content_dir / "assets";
  std::filesystem::path assets_dst = config.output_dir / "assets";

  if (std::filesystem::exists(assets_src)) {
    // remove assets in output dir if they exist
    if (std::filesystem::exists(assets_dst)) {
      std::filesystem::remove_all(assets_dst);
    }

    // copy assets directory recursively
    std::filesystem::create_directories(assets_dst);
    std::filesystem::copy(assets_src, assets_dst,
                          std::filesystem::copy_options::recursive);

    std::cout << "Copied assets directory\n";
  }
}

int main(int argc, char* argv[]) {
  try {
    // parse command line arguments
    Config config = parse_args(argc, argv);
    std::cout << "Processing markdown files from: " << config.content_dir
              << "\n"
              << "Generating HTML files to: " << config.output_dir << "\n";
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    print_usage();
    return 1;
  }
}
