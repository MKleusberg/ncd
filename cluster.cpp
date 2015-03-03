#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <fstream>
#include <future>
#include <mutex>
#include <queue>
#include <chrono>
#include <boost/filesystem.hpp>
#include <lzma.h>
using namespace boost;

typedef std::pair<unsigned int, unsigned int> matrix_index;
std::vector<std::string> files;
std::vector<std::string> filenames;
std::mutex mutex_readfile;
std::mutex mutex_todopop;
std::mutex mutex_resultpush;

unsigned int compressed_size(const std::string& data)
{
	std::string result;
	result.resize(data.size() + (data.size() >> 2) + 128);
	size_t out_pos = 0;

	if(lzma_easy_buffer_encode(
				   9,
				   LZMA_CHECK_NONE,
				   NULL,
				   reinterpret_cast<uint8_t*>(const_cast<char*>(data.data())),
				   data.size(),
				   reinterpret_cast<uint8_t*>(&result[0]),
				   &out_pos,
				   result.size()
				  ) != LZMA_OK)
	{
		std::cerr << "error compressing data!" << std::endl;
	}

	//result.resize(out_pos);
	return out_pos;
}

std::string& readfile(const std::string& filename)
{
	static std::map<std::string, std::string> cache;

	std::lock_guard<std::mutex> lock(mutex_readfile);

	if(cache[filename].empty())
	{
		std::ifstream file(filename);
		std::string str;

		file.seekg(0, std::ios::end);
		str.reserve(file.tellg());
		file.seekg(0, std::ios::beg);

		str.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

		cache[filename] = str;
	}

	return cache[filename];
}

unsigned int compressed_file_size(unsigned int file)
{
	static std::map<unsigned int, unsigned int> compressed_file_sizes;

	if(!compressed_file_sizes[file])
		compressed_file_sizes[file] = compressed_size(readfile(files.at(file)));

	return compressed_file_sizes[file];
}

float ncd(unsigned int file1, unsigned int file2)
{
	static std::map<matrix_index, float> compressed_combined_file_sizes;

	matrix_index cache_index = matrix_index(file2, file1);

	if(file1 == file2)
	{
		return 0.0f;
	} else if(compressed_combined_file_sizes[cache_index]) {
		return compressed_combined_file_sizes[cache_index];
	} else {
		float c = compressed_size(readfile(files.at(file1)) + readfile(files.at(file2)));
		float result = (c - std::min(compressed_file_size(file1), compressed_file_size(file2))) / std::max(compressed_file_size(file1), compressed_file_size(file2));

		compressed_combined_file_sizes[matrix_index(file1, file2)] = result;
		return result;
	}
}

void threadfunction(std::queue<matrix_index>& todo, std::map<matrix_index, float>& results)
{
	while(true)
	{
		mutex_todopop.lock();
		bool done = todo.empty();
		if(done)
		{
			mutex_todopop.unlock();
			return;
		}
		matrix_index index = todo.front();
		todo.pop();
		mutex_todopop.unlock();

		float result = ncd(index.first, index.second);

		mutex_resultpush.lock();
		results[index] = result;
		mutex_resultpush.unlock();
	}
}

void printstatus(std::queue<matrix_index>& todo)
{
	while(!todo.empty())
	{
		std::cout << "\r" << static_cast<int>((1 - static_cast<float>(todo.size()) / (files.size()*files.size())) * 100) << "% done" << std::flush;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	std::cout << "\rDone          " << std::endl;
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		std::cerr << "Usage:" << std::endl << argv[0] << " input-dir output-file" << std::endl;
		return 1;
	}

	filesystem::path input_path(argv[1]);
	if(!filesystem::exists(input_path) || !filesystem::is_directory(input_path))
	{
		std::cerr << argv[1] << " is not a directory" << std::endl;
		return 1;
	}
	if(filesystem::is_empty(input_path))
	{
		std::cerr << argv[1] << " is empty" << std::endl;
		return 1;
	}

	for(filesystem::directory_iterator it(input_path);it!=filesystem::directory_iterator();++it)
	{
		files.push_back(it->path().string());
		filenames.push_back(it->path().stem().string());
	}

	std::ofstream out;
	out.open(argv[2]);
	if(out.fail())
	{
		std::cerr << "Could not open output file for writing" << std::endl;
		return 1;
	}
	out << "    ";
	for(unsigned int i=0;i<files.size();i++)
		out << "\"" << filenames.at(i) << "\" ";
	out << std::endl;

	std::cout << "Calculating NCDs..." << std::endl;
	std::queue<matrix_index> indices;
	for(unsigned int i=0;i<files.size();i++)
		for(unsigned int j=0;j<files.size();j++)
			indices.push(matrix_index(i, j));
	std::map<matrix_index, float> results;
	std::vector<std::thread> threads;
	threads.push_back(std::thread(printstatus, std::ref(indices)));
	for(unsigned int i=0;i<std::thread::hardware_concurrency()*3;i++)
		threads.push_back(std::thread(threadfunction, std::ref(indices), std::ref(results)));
	for(unsigned int i=0;i<threads.size();i++)
		threads.at(i).join();
	for(unsigned int i=0;i<files.size();i++)
	{
		out << "\"" << filenames.at(i) << "\" ";

		for(unsigned j=0;j<files.size();j++)
			out << std::showpoint << std::left << std::setfill('0') << std::setw(8) << results[matrix_index(i, j)] << " ";
		out << std::endl;
	}
}
