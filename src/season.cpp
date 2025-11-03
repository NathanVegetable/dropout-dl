//
// Created by moss on 9/29/22.
//

#include "season.h"

namespace dropout_dl {
	std::vector<std::string> season::get_episode_urls() {
		std::vector<std::string> urls;
		const std::string pattern = R"(class="browse-item-link" data-track-event="site_video")";

		// Helper lambda to extract URLs from page data
		auto extract_urls_from_page = [&](const std::string& page_data) {
			for (size_t i = 0; i < page_data.size(); i++) {
				if (substr_is(page_data, i, pattern)) {
					// Search backwards for the href attribute
					for (int j = i; j >= 0 && j > i - 200; j--) {
						if (substr_is(page_data, j, "href=\"")) {
							// Extract URL
							size_t url_start = j + 6; // Skip 'href="'
							size_t url_end = page_data.find('"', url_start);
							if (url_end != std::string::npos) {
								std::string url = page_data.substr(url_start, url_end - url_start);
								urls.push_back(url);
								break;
							}
						}
					}
				}
			}
		};

		// Extract URLs from first page
		extract_urls_from_page(this->page_data);

		// Handle pagination - check for additional pages
		long status_code = -1;
		int page_index = 2;
		while (true) {
			std::string next_page_url = this->url + "?page=" + std::to_string(page_index);
			std::string next_page_data = get_generic_page(next_page_url, this->url, &status_code);

			if (status_code != 200) {
				break;
			}

			extract_urls_from_page(next_page_data);
			page_index++;
		}

		return urls;
	}

	int season::get_season_number(const std::string& url) {
		std::string reversed_number = "";
		for (int i = url.length() - 1; i >= 0 && url[i] != ':'; i--) {
			if (isdigit(url[i])) {
				reversed_number += url[i];
			}
		}
		std::string number = "";
		for (int i = reversed_number.length() - 1; i >= 0; i--) {
			number += reversed_number[i];
		}

		return std::stoi(number);
	}

} // dropout_dl
