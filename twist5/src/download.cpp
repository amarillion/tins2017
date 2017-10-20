#include "download.h"
#include <string.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include "fileutil.h"
#include <iostream>

#ifdef ALLEGRO_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef USE_CURL
#include <curl/curl.h>
#endif

using namespace std;

#ifdef USE_CURL
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
int progress_callback(void *clientp,   curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow);
#endif

class DownloadAgentImpl : public DownloadAgent
{
private:
	string userAgent;

	bool interrupted = false;
	ALLEGRO_MUTEX *mutex;
public:
	DownloadAgentImpl() : userAgent("libcurl-twist/1.0")
	{
		mutex = al_create_mutex();
	}
	~DownloadAgentImpl()
	{
		al_destroy_mutex(mutex);
	}

	/** thread safe */
	bool isInterrupted()
	{
		bool result;
		al_lock_mutex(mutex);
		result = interrupted;
		al_unlock_mutex(mutex);
		return result;
	}

	/** thread safe */
	void setInterrupted(bool value)
	{
		al_lock_mutex(mutex);
		interrupted = value;
		al_unlock_mutex(mutex);
	}

	/** thread safe */
	virtual void interrupt()
	{
		setInterrupted(true);
	}

	virtual void setUserAgent(const string &value) override
	{
		userAgent = value;
	}

	virtual Response downloadToFile(const string &url, const string &fname) override
	{
		setInterrupted(false);

		Response result = Response();
		result.valid = false;
	#ifdef USE_CURL

		string tempnam = fname + ".tmp";
		
		CURL *curl;
		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300L); // in seconds
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // data writing callback
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback); // progress callback allows us to interrupt
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this); // reference to this for progress callback
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0); // enable progress callback
		curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());

		CURLcode res;
		FILE *pagefile;
		pagefile = fopen(tempnam.c_str(), "wb");
		if(pagefile) {

			/* write the page body to this file handle */
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, pagefile);

			/* get it! */
			res = curl_easy_perform(curl);

			/* close the header file */
			fclose(pagefile);
			
			result.resultCode = res;
			if(res != CURLE_OK)
			{
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
						curl_easy_strerror(res));

				result.msg = curl_easy_strerror(res);

				if( remove( tempnam.c_str() ) != 0 )
					perror( "Error deleting temporary file" );
				else
					puts( "Temporary file successfully deleted" );
			}
			else
			{
				result.valid = true;
#ifdef ALLEGRO_WINDOWS
				// on windows, use MoveFileEx if we want to overwrite existing file
				int err = MoveFileEx (tempnam.c_str(), fname.c_str(), MOVEFILE_REPLACE_EXISTING);
				if (err == 0)
				{
					cout << "MoveFileEx failed: " << GetLastError() << endl;
				}
#else
				// on *nix, rename also overwrites target file
				int err = rename (tempnam.c_str(), fname.c_str());
				if (err != 0)
				{
					perror ("Error renaming file: ");
				}
#endif
				result.fname = fname;
			}
		}
		else
		{
			cout << "Could not open file: " << tempnam << endl;
		}
		
		/* always cleanup */
		curl_easy_cleanup(curl);

		curl_global_cleanup();

	#else
		result.msg = "libCurl not linked, compile with -DUSE_CURL or contact developers";
	#endif
	  return result;
	}

};

#ifdef USE_CURL
// copied from curl example: http://curl.haxx.se/libcurl/c/url2file.html
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
  return written;
}

int progress_callback(void *clientp,   curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow)
{
	// the callback is called about once per second, even when nothing is downloading.
	// we use it to signal thread interruption.
	auto downloader = ((DownloadAgentImpl*)clientp);
	return downloader->isInterrupted() ? 1 : 0;
}

#endif

shared_ptr<DownloadAgent> DownloadAgent::newInstance() {
	return make_shared<DownloadAgentImpl>();
}
