//
// Created by moss on 9/29/22.
//
#pragma once

#include <iostream>
#include <vector>

#include <chrono> // for to go sleepy
#include <thread> // for to go sleepy

#include "episode.h"

namespace dropout_dl {

	class season {
		public:
			/// The name of the season
			std::string name;
			/// The name of the series
			std::string series_name;
			/// The number of the season
			int season_number;
			/// The link to the season page
			std::string url;
			/// The season page data
			std::string page_data;
			/// The list of all episode URLs in the season
			std::vector<std::string> episode_urls;
			/// Whether or not to download captions
			bool download_captions;
			/// Whether to skip the video and only download captions
			bool download_captions_only;
			/// Ammount of time between downloading episodes
			uint32_t rate_limit;

			/**
			 *
			 * @return A vector of all episode URLs in the season
			 *
			 * Gets all the episode URLs from the season page
			 */
			std::vector<std::string> get_episode_urls();

			/**
			 *
			 * @param url - The url of the season
			 * @return The number of the season
			 *
			 * Gets the canonical number of the season for the url. This is sometimes different from the displayed number because of special seasons.
			 */
			 static int get_season_number(const std::string& url);

			/**
			 *
			 * @param url - The url to the webpage of the season
			 * @param name - The name of the season
			 * @param session_cookie - The cookie used to authenticate
			 * @param series_name - The name of the series
			 *
			 * Creates a season object and populates the needed information.
			 */
			season(const std::string& url, const std::string& name, const cookie& session_cookie, const std::string& series_name = "", bool download_captions = false, bool download_captions_only = false, const uint32_t rate_limit = 2000) {
				this->url = url;
				this->download_captions = download_captions;
				this->download_captions_only = download_captions_only;
				this->season_number = get_season_number(this->url);
				this->rate_limit = rate_limit;
				this->name = "Season " + std::to_string(this->season_number);
				this->series_name = series_name;
				std::cout << series_name << ": " << name << ": " << "\n";
				this->page_data = get_generic_page(url);
				this->episode_urls = get_episode_urls();
			}
	};

} // dropout_dl

