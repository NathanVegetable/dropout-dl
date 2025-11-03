#include <iostream>

#include "series.h"
#include "login.h"
#include <regex>

#ifdef DROPOUT_DL_SQLITE
#include <sqlite3.h>
#endif

namespace dropout_dl {

	/**
	 * A class for handling and storing the program arguments.
	 */
	class options {
	public:

		std::string url;
		bool verbose = false;
		bool force_cookies = false;
		bool browser_cookies = false;
		bool is_series = false;
		bool is_season = false;
		bool is_episode = false;
		bool download_captions = false;
        bool download_captions_only = false;
		bool keep_segment_files = false;
		bool list_urls = false;
		uint32_t rate_limit = 2000; // rate limit in ms
		std::string quality;
		std::string filename;
		std::string login_file = "login"; /// Default incase the option is not used
		std::string output_directory;
		std::string episode;
		std::string container_format = "mp4"; /// Default container format
		cookie session_cookie;

		/**
		 *
		 * @param argc - The number of provided program arguments
		 * @param argv - The provided program arguments
		 * @return A vector of arguments in the c++ string format
		 *
		 * Converts the C style program arguments to a vector of strings
		 */
		static std::vector<std::string> convert_program_args(int argc, char** argv) {
			std::vector<std::string> out;
			for (int i = 1; i < argc; i++) {
				out.emplace_back(argv[i]);
			}
			return out;
		}

		/**
		 *
		 * @param argc - The number of provided program arguments
		 * @param argv - The provided program arguments
		 *
		 * Parses and handles the program arguments and creates an options object.
		 */
		options(int argc, char** argv) {
			std::vector<std::string> args = convert_program_args(argc, argv);

			for (int i = 0; i < args.size(); i++) {
				std::string arg = args[i];

				if (arg[0] != '-') {
					url = arg;
					continue;
				}
				if (arg[1] == '-') {
					// Full names. prefixed by `--`
					arg = arg.substr(2);
				}
				else {
					// Shorthands. prefixed by `-`
					arg = arg.substr(1);
				}
				if (arg == "verbose" || arg == "v") {
					verbose = true;
				} else if (arg == "quality" || arg == "q") {
					if (i + 1 >= args.size()) {
						std::cerr << "ARGUMENT PARSE ERROR: --quality used with too few following arguments\n";
						exit(8);
					}
					quality = args[++i];
				}
				else if (arg == "browser-cookies" || arg == "bc") {
					browser_cookies = true;
				}
				else if (arg == "force-cookies") {
					if (i + 2 >= args.size()) {
						std::cerr << "ARGUMENT PARSE ERROR: --force-cookies used with too few following arguments\n";
						exit(8);
					}
					session_cookie = cookie(args[++i]);
					force_cookies = true;
				}
				else if (arg == "output" || arg == "o") {
					if (i + 1 >= args.size()) {
						std::cerr << "ARGUMENT PARSE ERROR: --output used with too few following arguments\n";
						exit(8);
					}
					filename = args[++i];
				}
				else if (arg == "output-directory" || arg == "d") {
					if (i + 1 >= args.size()) {
						std::cerr << "ARGUMENT PARSE ERROR: --output-directory used with too few following arguments\n";
						exit(8);
					}
					output_directory = args[++i];
				}
				else if (arg == "rate" || arg == "r") {
					if (i + 1 >= args.size()) {
						std::cerr << "ARGUMENT PARSE ERROR: --rate used with too few following arguments\n";
						exit(8);
					}
					rate_limit = std::stoi(args[++i]);
				}
				else if (arg == "format" || arg == "f") {
				if (i + 1 >= args.size()) {
					std::cerr << "ARGUMENT PARSE ERROR: --format used with too few following arguments\n";
					exit(8);
				}
				container_format = args[++i];
				if (container_format != "mp4" && container_format != "mkv") {
					std::cerr << "ARGUMENT PARSE ERROR: --format must be either 'mp4' or 'mkv'\n";
					exit(8);
				}
			}
			else if (arg == "login-file" || arg == "lf") {
					if (i + 1 >= args.size()) {
						std::cerr << "ARGUMENT PARSE ERROR: --login-file used with too few following arguments\n";
						exit(8);
					}
					login_file = args[++i];
				}
				else if (arg == "series" || arg == "S") {
					is_series = true;
				}
				else if (arg == "season" || arg == "s") {
					is_season = true;
				}
				else if (arg == "episode" || arg == "e") {
					is_episode = true;
				}
				else if (arg == "captions" || arg == "c") {
					download_captions = true;
				}
                else if (arg == "captions-only" || arg == "co") {
					download_captions_only = true;
				}
				else if (arg == "list-urls" || arg == "l") {
					list_urls = true;
				}
				//// TODO: Add support for keeping m4a and m4s files
				else if (arg == "help" || arg == "h") {
					std::cout << "Usage: dropout-dl [OPTIONS] <url> [OPTIONS]\n"
								 "\n"
								 "Options:\n"
								 "\t--help              -h   Display this message\n"
								 "\t--quality           -q   Set the quality of the downloaded video. Special Qualities:"
								 "\t                             'all' - download all qualities and place them into separate folders\n"
								 "\t                             'highest' - download the highest possible quality (default)\n"
								 "\t                             'lowest' - download the lowest possible quality\n"
								 "\t--output            -o   Set the output filename. Only works for single episode downloads\n"
								 "\t--output-directory  -d   Set the directory where files are output\n"
							 "\t--format            -f   Set the output container format: 'mp4' (default) or 'mkv'\n"
								 "\t--verbose           -v   Display debug information while running\n"
								 "\t--browser-cookies   -bc  Use cookies from the browser placed in 'firefox_profile' or 'chrome_profile'\n"
								 "\t--rate              -r   Set the ammount of time in milliseconds between getting episodes\n"
								 "\t                             Only affects series and season downloads. Defaults to 2000\n"
								 "\t--force-cookies          Interpret the next argument as the session cookie\n"
								 "\t--login-file        -lf  Use the next argument as the path to the login file\n"
								 "\t--series            -S   Interpret the url as a link to a series and download all episodes from all seasons\n"
								 "\t--season            -s   Interpret the url as a link to a season and download all episodes from all seasons\n"
								 "\t--episode           -e   Interpret the url as a link to a single episode\n"
								 "\t--captions          -c   Download the captions along with the episode. Overridden by --captions-only if set.\n"
                                 "\t--captions-only     -co  Download the captions only, without the episode\n"
								 "\t--list-urls         -l   List all episode URLs that would be downloaded without downloading\n";

					exit(0);
				}
			}

			if (output_directory.empty()) {
				output_directory = ".";
			}

			if (browser_cookies && force_cookies) {
				std::cerr << "ARGUMENT PARSE ERROR: Cannot use browser cookies and forced cookies\n";
				// Default to browser cookies.
				force_cookies = false;
			}

			if ((is_season && is_series) || (is_season && is_episode) || (is_series && is_episode)) {
				std::cerr << "ARGUMENT PARSE ERROR: Mulitple parse type arguments used\n";
			}
			if (quality.empty()) {
				quality = "highest";
			}

			if (!(is_season || is_series || is_episode)) {
				std::regex season_regex("season:\\d+\\/?$", std::regex::ECMAScript);
				std::regex episode_regex("/videos/", std::regex::ECMAScript);
				if (std::regex_search(url, season_regex)) {
					is_season = true;
				}
				else if (std::regex_search(url, episode_regex)) {
					is_episode = true;
				}
				else {
					is_series = true;
				}
			}
		}
	};
}

#ifdef DROPOUT_DL_SQLITE
/**
 *
 * @param firefox_profile_path - The path to a firefox profile
 * @param verbose - Whether or not to be verbose
 * @return A vector of cookies
 *
 * Gets the needed cookies from the firefox sqlite database associated with the path provided.
 */
dropout_dl::cookie get_cookies_from_firefox(const std::filesystem::path& firefox_profile_path, bool verbose = false) {

	std::fstream firefox_profile_file(firefox_profile_path);
	std::string firefox_profile;

	dropout_dl::cookie session("_session");

	std::vector<dropout_dl::cookie> out;

	firefox_profile_file >> firefox_profile;

	if (!std::filesystem::is_directory(firefox_profile)) {
		std::cerr << "FIREFOX COOKIE ERROR: Attempted to get cookies from firefox without profile." << std::endl;
		exit(4);
	}


	sqlite3 *db;

	if (verbose) {
		std::cout << "Getting firefox cookies from firefox sqlite db\n";
	}

	/// Firefox locks the database so we have to make a copy.
	if (!std::filesystem::is_directory("tmp"))
		std::filesystem::create_directories("tmp");
	std::filesystem::remove("tmp/firefox_cookies.sqlite");
	std::filesystem::copy_file(firefox_profile + "/cookies.sqlite", "tmp/firefox_cookies.sqlite");

	int rc = sqlite3_open("tmp/firefox_cookies.sqlite", &db);
	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << '\n';
		exit(1);
	} else {
		if (verbose) {
			std::cout << "Firefox database opened successfully\n";
		}
	}

	std::string len;

	session.get_value_from_db(db, "FROM moz_cookies WHERE host LIKE '%dropout.tv%'", "value");

	sqlite3_close(db);

	std::filesystem::remove("tmp/firefox_cookies.sqlite");

	if (std::filesystem::is_empty("tmp")) {
		std::filesystem::remove("tmp/");
	}

	if (verbose) {
		std::cout << session.name << ": " << session.len << ": " << session.value << '\n';
	}

	return session;
}

#ifdef DROPOUT_DL_GCRYPT
/**
 *
 * @param chrome_profile_path - The path to a chrome profile
 * @param verbose - Whether or not to be verbose
 * @return A vector of cookies
 *
 * Gets the needed cookies from the chrome sqlite database associated with the path provided and decrypts them using the libgcrypt library.
 * This function does not work for windows and must be modified slightly for mac os.
 * For mac os the calls to cookie::chrome_decrypt must be passed the parameters detailed in it's documentation.
 */
dropout_dl::cookie get_cookies_from_chrome(const std::filesystem::path& chrome_profile_path, bool verbose = false) {

	std::fstream chrome_profile_file(chrome_profile_path);
	std::string chrome_profile;

	dropout_dl::cookie session("_session");

	std::vector<dropout_dl::cookie> out;

	getline(chrome_profile_file, chrome_profile);

	if (std::filesystem::is_directory(chrome_profile)) {

		sqlite3 *db;

		if (verbose) {
			std::cout << "Getting chrome cookies from chrome sqlite db\n";
		}

		int rc = sqlite3_open((chrome_profile + "/Cookies").c_str(), &db);
		if (rc) {
			std::cerr << "Can't open database: " << sqlite3_errmsg(db) << '\n';
			exit(1);
		} else {
			if (verbose) {
				std::cout << "Chrome database opened successfully\n";
			}
		}

		std::string len;

		session.get_value_from_db(db, "FROM cookies WHERE host_key LIKE '%dropout.tv%'", "encrypted_value");

		sqlite3_close(db);

	}
	else {
		std::cerr << "CHROME COOKIE ERROR: Attempted to get cookies from chrome without profile." << std::endl;
		exit(4);
	}

	session.chrome_decrypt();

	if (verbose) {
		std::cout << session.name << ": " << session.len << ": " << session.value << '\n';
	}

	return session;
}
#endif
#endif

/**
 *
 * @param verbose - Whether or not to be verbose
 * @return A vector of cookies
 *
 * Determines whether to get cookies from firefox or chrome. This function should not be run if cookies are forced using the `--force-cookies` option.
 * This function checks firefox first so if both firefox and chrome profiles are provided it will use firefox.
 */
dropout_dl::cookie get_cookie_from_browser(bool verbose = false) {

	std::filesystem::path firefox_profile("firefox_profile");
	std::filesystem::path chrome_profile("chrome_profile");

	if (std::filesystem::exists(firefox_profile)) {

		#ifdef DROPOUT_DL_SQLITE
		return get_cookies_from_firefox(firefox_profile, verbose);
		#else
				std::cout << "WARNING: Firefox profile file exists but sqlite is not installed" << std::endl;
		#endif
	}
	if (std::filesystem::exists(chrome_profile)) {
		#if defined(DROPOUT_DL_GCRYPT) & defined(DROPOUT_DL_SQLITE)
		return get_cookies_from_chrome(chrome_profile, verbose);
		#else
		std::cout << "WARNING: Chrome profile file exists but libgcrypt or sqlite is not installed" << std::endl;
		#endif
	}

	std::cerr << "ERROR: dropout.tv cookies could not be found" << std::endl;
	exit(7);
}


int main(int argc, char** argv) {
	dropout_dl::options options(argc, argv);

	if (options.verbose) {
		std::cout << "quality: " << options.quality << std::endl;
		std::cout << "verbose: " << options.verbose << std::endl;
		std::cout << "url: \"" << options.url << '"' << std::endl;
		std::cout << "rate: " << options.rate_limit << std::endl;
		std::cout << "captions: " << options.download_captions << std::endl;
	}

	std::string firefox_profile;
	std::string chrome_profile;

	std::string video_data;

	if (options.url.empty()) {
		std::cout << "Enter episode url: ";
		std::cin >> options.url;
	}
	else if (options.verbose) {
		std::cout << "Got episode url: " << options.url << " from program arguments\n";
	}

	if (options.browser_cookies) {
		options.session_cookie = get_cookie_from_browser(options.verbose);
	}
	else if (!options.force_cookies) {
		std::string session;
		dropout_dl::login::get_cookies(session, options.login_file);

		options.session_cookie = dropout_dl::cookie("_session", session);
	}

	// Step 1: Collect all episodes we need to download into a single vector
	std::vector<dropout_dl::episode> episodes_to_download;
	std::string output_directory = options.output_directory;

	if (options.is_series) {
		if (options.verbose) {
			std::cout << "Getting series episodes\n";
		}

		// Extract series name from URL
		std::string series_name = "";
		size_t last_slash = options.url.find_last_of('/');
		if (last_slash != std::string::npos) {
			series_name = options.url.substr(last_slash + 1);
		}
		output_directory = options.output_directory + "/" + series_name;

		// Try seasons sequentially until we hit a 404
		for (int season_num = 1; season_num <= 20; season_num++) {
			std::string season_url = options.url + "/season:" + std::to_string(season_num);

			if (options.verbose) {
				std::cout << "Checking season " << season_num << ": " << season_url << '\n';
			}

			// Check if season exists
			long status_code = -1;
			std::string page_data = dropout_dl::get_generic_page(season_url, "", &status_code);

			if (status_code != 200) {
				if (options.verbose) {
					std::cout << "Season " << season_num << " not found, stopping\n";
				}
				break;
			}

			// Create season object to extract episode URLs (constructor populates episode_urls)
			dropout_dl::season season(season_url, "", options.session_cookie, series_name, options.download_captions, options.download_captions_only, options.rate_limit);

			// Create episode objects from URLs and add to download list
			// Episode numbers will be extracted from each episode's page
			if (options.verbose) {
				std::cout << "Main: Season " << season_num << " - Creating episode objects for " << season.episode_urls.size() << " URLs\n";
			}
			int ep_index = 0;
			for (const auto& episode_url : season.episode_urls) {
				if (options.verbose) {
					std::cout << "Main: Season " << season_num << " - Creating episode object " << (ep_index + 1) << "/" << season.episode_urls.size() << " for URL: " << episode_url << '\n';
				}
				dropout_dl::episode ep(episode_url, options.session_cookie, series_name, "Season " + std::to_string(season_num), 0, season_num, options.verbose, options.download_captions, options.download_captions_only);
				if (options.verbose) {
					std::cout << "Main: Episode object created successfully, adding to download list\n";
				}
				episodes_to_download.push_back(ep);
				if (options.verbose) {
					std::cout << "Main: Episode added to download list\n";
				}
				ep_index++;
			}
			if (options.verbose) {
				std::cout << "Main: Season " << season_num << " - All episode objects created\n";
			}
		}
	}
	else if (options.is_season) {
		if (options.verbose) {
			std::cout << "Getting season episodes\n";
		}

		// Extract series name from URL
		if (options.verbose) {
			std::cout << "Main: Extracting series name from URL: " << options.url << '\n';
		}
		std::string series_name = "";
		size_t last_slash = options.url.find_last_of('/');
		if (options.verbose) {
			std::cout << "Main: last_slash = " << last_slash << '\n';
		}
		if (last_slash != std::string::npos && last_slash > 0) {
			size_t second_last_slash = options.url.find_last_of('/', last_slash - 1);
			if (options.verbose) {
				std::cout << "Main: second_last_slash = " << second_last_slash << '\n';
			}
			if (second_last_slash != std::string::npos) {
				series_name = options.url.substr(second_last_slash + 1, last_slash - second_last_slash - 1);
				if (options.verbose) {
					std::cout << "Main: Extracted series_name = '" << series_name << "'\n";
				}
			}
		}
		if (series_name.empty()) {
			if (options.verbose) {
				std::cout << "Main: series_name is empty, using 'unknown-series' as fallback\n";
			}
			series_name = "unknown-series";
		}
		output_directory = options.output_directory + "/" + series_name;
		if (options.verbose) {
			std::cout << "Main: output_directory = " << output_directory << '\n';
		}

		// Create season object to extract episode URLs (constructor populates episode_urls)
		dropout_dl::season season(options.url, "", options.session_cookie, series_name, options.download_captions, options.download_captions_only, options.rate_limit);

		// Create episode objects from URLs and add to download list
		// Episode numbers will be extracted from each episode's page
		if (options.verbose) {
			std::cout << "Main: Creating episode objects for " << season.episode_urls.size() << " URLs\n";
		}
		int ep_index = 0;
		for (const auto& episode_url : season.episode_urls) {
			if (options.verbose) {
				std::cout << "Main: Creating episode object " << (ep_index + 1) << "/" << season.episode_urls.size() << " for URL: " << episode_url << '\n';
			}
			dropout_dl::episode ep(episode_url, options.session_cookie, series_name, "Season " + std::to_string(season.season_number), 0, season.season_number, options.verbose, options.download_captions, options.download_captions_only);
			if (options.verbose) {
				std::cout << "Main: Episode object created successfully, adding to download list\n";
			}
			episodes_to_download.push_back(ep);
			if (options.verbose) {
				std::cout << "Main: Episode added to download list\n";
			}
			ep_index++;
		}
		if (options.verbose) {
			std::cout << "Main: All episode objects created\n";
		}
	}
	else if (options.is_episode) {
		if (options.verbose) {
			std::cout << "Getting single episode\n";
		}

		// Create the episode and add it to our download list
		dropout_dl::episode ep(options.url, options.session_cookie, options.verbose, options.download_captions, options.download_captions_only);
		episodes_to_download.push_back(ep);
	}
	else {
		std::cerr << "ERROR: Could not determine parsing type\n";
		return 1;
	}

	// Step 2: Download all episodes using the single download code path
	if (episodes_to_download.empty()) {
		std::cout << "No episodes found to download\n";
		return 0;
	}

	std::cout << "Found " << episodes_to_download.size() << " episode(s) to download\n";

	// If list_urls is enabled, show the actual file paths that will be created
	if (options.list_urls) {
		std::cout << "\nEpisodes that would be downloaded:\n";
		std::cout << "Base directory: " << output_directory << "\n\n";

		for (const auto& ep : episodes_to_download) {
			// Use the episode's get_download_path() method to ensure consistency with actual downloads
			std::string file_path = ep.get_download_path(output_directory, options.container_format);

			std::cout << "  " << file_path << "\n";
			std::cout << "    URL: " << ep.episode_url << "\n\n";
		}
		return 0;
	}

	// Create output directory if needed
	if (!std::filesystem::is_directory(output_directory)) {
		std::filesystem::create_directories(output_directory);
		if (options.verbose) {
			std::cout << "Created output directory: " << output_directory << '\n';
		}
	}

	// Download each episode
	for (auto& ep : episodes_to_download) {
		ep.download(options.quality, output_directory, options.filename, options.container_format);

		// Rate limit between episodes
		if (&ep != &episodes_to_download.back()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(options.rate_limit));
		}
	}


	return 0;
}
