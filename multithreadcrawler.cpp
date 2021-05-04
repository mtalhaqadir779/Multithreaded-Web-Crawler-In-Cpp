/* Semester Project ==> Web Spider

Muhammad Yousaf Saddique
Muhammad Talha Qadir
Muhammad Haider Ahmed

*/

// All used Libraries

#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <list>
#include <dirent.h>	 // used for directory access operations
#include <pthread.h> // for multithreading programming
#include <unistd.h>	 // provides access to the POSIX Operating System API

// libcurl (C library) alternative/wrapper of C++

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

// Max Length of URL to store

#define MAX_LEN_URL 255

using namespace std;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

// Structure of URL's to be visited/downloaded
struct WebUrl
{
	string url;
	int to_visit;
};

// CLass used by HTML Parser class to get the url to download
class DownloadList
{

public:
	void insert_url(string element)
	{
		//called from the HTML Parser to put a new url in the list of url to download

		if (!strcmp((element.substr(0, 7)).c_str(), "http://") /* || !strcmp((element.substr(0, 8)).c_str(), "https://")*/)
		{

			// found 0 if element not found in list, 1 if element found in list already

			found = check_url(element);
			if (found == 0)
			{
				WebUrl newelement;
				newelement.url = element;
				newelement.to_visit = 1;
				url_list.push_back(newelement);
			}
		}
	}

	string retrieve_url()
	{
		//called by the DownloaderObj returns an url to download

		if (url_list.size() == 0)
		{

			cout << "FETCHER is waiting for a url to fetch from Parser" << endl;
			while (url_list.size() == 0)
				;
		}

		do
		{

			iter = url_list.begin();
			while (iter != url_list.end() && iter->to_visit == 0)
			{

				iter++;
			}

			if (iter != url_list.end())
			{

				newurl = iter->url;
				iter->to_visit = 0;
			}
			else
			{

				cout << "\nHTML Parser Parsing URL and Downloader is waiting for new URLs.\n"
					 << endl;
				sleep(5);
			}

		} while (iter == url_list.end());

		return newurl;
	}

private:
	int found;
	list<WebUrl> url_list;
	string newurl;
	list<WebUrl>::iterator iter;

	int check_url(string element)
	{
		//checks if the url to insert is allready in the list and return 1 if so

		for (list<WebUrl>::const_iterator iter = url_list.begin(); iter != url_list.end(); ++iter)
		{

			if (!strcmp((iter->url).c_str(), element.c_str()))
			{

				return 1;
			}
		}

		return 0;
	}
};

class Downloader
{
public:
	string url, title;
	char cleanurl[MAX_LEN_URL];
	int i, j;

	void WebpageDownload(DownloadList *listObjPointer)
	{
		//saves the html of the downloaded url in a document contained in the directory "documents"

		url = listObjPointer->retrieve_url();
		cout << "\nDownloader is downloading url " << url.c_str() << endl
			 << endl;
		j = 0;
		for (i = 0; i < url.length(); i++)
		{

			if (url[i] != '/' && url[i] != '&' && url[i] != ':' && url[i] != '%' && url[i] != '=' && url[i] != '?' && url[i] != '.')
			{

				cleanurl[j] = url[i];
				j++;
			}
		}

		cleanurl[j] = '\0';
		title = string(cleanurl);
		try
		{

			pthread_mutex_lock(&mutex1);

			// Calling cUrlpp::Initialize using Cleanup class Constructor
			curlpp::Cleanup myCleanup;

			cout << "\nMUTEX LOCK ==> DOWNLOADER WORKING"
				 << endl;

			// Writing informtion to downloaded file in Documents folder
			std::ofstream saveddoc;
			saveddoc.open(("documents/" + title).c_str());

			if (!saveddoc.good())
			{
				cout << "Could not write file!" << endl;
				exit(1);
			}

			// Retreive the content of a web page and put it in a stream.
			saveddoc << curlpp::options::Url(url.c_str()) << endl;

			saveddoc.close();
			pthread_mutex_unlock(&mutex1);
			cout << "\nMUTEX UNLOCK ==> DOWNLOADER FREE"
				 << endl;
		}
		catch (curlpp::RuntimeError &e)
		{

			cout << e.what() << endl;
			// Frees the mutex in case of exception
			pthread_mutex_unlock(&mutex1);
		}
		catch (curlpp::LogicError &e)
		{

			cout << e.what() << endl;
			// Frees the mutex in case of exception
			pthread_mutex_unlock(&mutex1);
		}
	}
};

class HtmlParser
{
public:
	char *var, *cstr, *checkstr;
	size_t initial_position, final_position;
	char url[MAX_LEN_URL];
	string doc_title, fileline, shortline;
	int i, j;
	char finalurl[MAX_LEN_URL];
	void ParseHtmlUrl(DownloadList *listObjPointer)
	{
		//takes a document from the directory "documents", parses it looking for tag <a> and gets the url contained in href=""

		do
		{

			ReadDir("documents");

			doc_title = doc_list.front();
			doc_list.pop_front();

			pthread_mutex_lock(&mutex1);
			cout << "MUTEX LOCK ==> HTML PARSER WORKING\n"
				 << endl;

			std::ifstream file;
			file.open(("documents/" + doc_title).c_str());

			if (!file.good())
			{
				cerr << "File '"
					 << "documents/" + doc_title << "' does not exist!" << endl;
				exit(1);
			}

			while (!file.eof())
			{

				getline(file, fileline);
				initial_position = fileline.find("<a");
				final_position = fileline.find("</a>");

				if (final_position == string::npos || initial_position == string::npos)
				{
					continue;
				}

				shortline = fileline.substr(initial_position, final_position - initial_position);

				cstr = new char[shortline.size() + 1];
				strcpy(cstr, shortline.c_str());

				var = strstr(cstr, "href=\"");
				if (var != NULL)
				{

					int i = 0;
					while (*var != '"')
					{
						var++;
					}

					var++;
					while (*var != '"' && i < MAX_LEN_URL - 1)
					{
						url[i] = *var;
						var++;
						i++;
					}

					url[i] = '\0';

					if (i < MAX_LEN_URL)
					{
						cout << "URL Extracted - " << url << " -" << endl;
						// open a file in write mode.
						ofstream outfile;
						outfile.open("output.txt", std::ios_base::app);
						outfile << url << endl;
						// close the opened file.
						outfile.close();
						listObjPointer->insert_url(url);
					}
				}

				delete[] cstr;
			}

			file.close();

			pthread_mutex_unlock(&mutex1);
			cout << "\nMUTEX UNLOCK ==> HTML PARSER FREE\n"
				 << endl;

			RemoveFile(doc_title);
			cout << "\n\nSuccessfully crawled the document " << doc_title << endl;

		} while (doc_list.size() != 0);
	}

private:
	list<string> doc_list;
	string line, filepath;
	DIR *dp;
	struct dirent *dirp;

	void ReadDir(string dir)
	{
		//reads the documents contained in the folder specified and pushes them in a list (for the ParseHtmlUrl method)

		int i;
		do
		{

			i = 0;
			doc_list.erase(doc_list.begin(), doc_list.end());
			dp = opendir(dir.c_str());
			if (dp == NULL)
			{

				cout << "Error opening directory" << dir << endl;
				exit(1);
			}

			while (dirp = readdir(dp))
			{

				i++;
				if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
					;
				else
				{

					filepath = dir + "/" + dirp->d_name;
					doc_list.push_back(dirp->d_name);
				}
			}

			closedir(dp);
			if (i == 2)
			{

				cout << "\nEmpty Directory, No documents to parse." << endl;
				sleep(5);
			}

		} while (i <= 2);
	}

	void RemoveFile(string filename)
	{
		//delete the file with the title in input

		pthread_mutex_lock(&mutex1);

		if (remove(("documents/" + filename).c_str()) != 0)
			perror("Error deleting file");

		pthread_mutex_unlock(&mutex1);
	}
};

// Routine that the thread executes
void *fetch_links(void *arg)
{
	//fetching thread

	Downloader DownloaderObj;
	DownloadList *listObjPointer;
	listObjPointer = ((DownloadList *)arg);

	while (true)
	{

		DownloaderObj.WebpageDownload(listObjPointer);
	}
}

// Main Program

int main()
{
	// Copy input into URL Array and Attach http:// to URL

	char URL[MAX_LEN_URL];

	cout << "Enter URL: ";
	cin >> URL;

	int size = strlen(URL);

	//	strcpy(URL, argv[1]);
	char as[262] = "http://";
	strcat(as, URL); //

	// Creates Object pointer of class DownloadList and points it to dynamically allocated object of same class

	DownloadList *listObjPointer;
	listObjPointer = new DownloadList();

	// Insert input URL to List

	listObjPointer->insert_url(as);

	HtmlParser parserObj;
	pthread_t tid;

	// Thread Creation

	if (pthread_create(&tid, NULL, fetch_links, listObjPointer) != 0)
	{
		// Thread created and goes to fectch the links to download

		cout << "Created New Thread" << endl;
	}

	while (1)
	{

		parserObj.ParseHtmlUrl(listObjPointer); //
	}
}
