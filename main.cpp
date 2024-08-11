#include <bits/stdc++.h>
#include <vector>
#include <queue>
#include <unordered_set>
#include <string>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <ctime>

using namespace std;

class SongQueue
{
private:
    int n, k;
    queue<string> songQueue;
    // Saare songs store karne ke liye
    vector<string> allSongs;
    // Map to track song positions
    unordered_map<string, int> songIndexMap;
    // Last k songs ko track karne ke liye
    deque<string> recentSongs;

public:
    SongQueue(int n, int k) : n(n), k(k) {}

    // Songs ko queue aur vector mein add karna
    void addSongs(const vector<string> &songs)
    {
        if (songs.size() != n)
        {
            throw invalid_argument("Number of songs provided does not match n");
        }

        songQueue = queue<string>();
        allSongs = songs;
        songIndexMap.clear(); // Clear the map before populating

        for (int i = 0; i < n; ++i)
        {
            songQueue.push(songs[i]);
            songIndexMap[songs[i]] = i; // Track song index
        }
    }

    // Nayi song ko add karna
    void addNewSong(const string &song)
    {
        if (songIndexMap.size() >= n)
        {
            throw overflow_error("Cannot add more than n songs");
        }
        songQueue.push(song);
        allSongs.push_back(song);
        songIndexMap[song] = allSongs.size() - 1; // Update song index
    }

    // Song ko remove karna
    void removeSong(const string &song)
    {
        auto it = songIndexMap.find(song);
        if (it != songIndexMap.end())
        {
            // Update allSongs and queue
            int index = it->second;
            allSongs.erase(allSongs.begin() + index);
            songIndexMap.erase(it);

            // Rebuild the songQueue from the remaining songs
            queue<string> tempQueue;
            for (const auto &s : allSongs)
            {
                tempQueue.push(s);
            }
            songQueue = tempQueue;
        }
        else
        {
            throw invalid_argument("Song not found in the playlist");
        }
    }

    // Next song ko fetch karna
    string getNextSong()
    {
        if (songQueue.empty())
        {
            throw out_of_range("No more songs in the queue");
        }

        string nextSong;
        unordered_set<string> recentSet(recentSongs.begin(), recentSongs.end());

        while (!songQueue.empty())
        {
            nextSong = songQueue.front();
            songQueue.pop();
            if (recentSet.find(nextSong) == recentSet.end())
            {
                break;
            }
            else
            {
                songQueue.push(nextSong);
            }
        }

        if (recentSongs.size() == k)
        {
            recentSongs.pop_front(); // Remove old song if limit reached
        }
        recentSongs.push_back(nextSong);

        return nextSong;
    }

    // Playlist ko shuffle karna
    void shufflePlaylist()
    {
        srand(time(0)); // Random seed set karna
        random_shuffle(allSongs.begin(), allSongs.end());
        while (!songQueue.empty())
        {
            songQueue.pop();
        }
        for (const auto &song : allSongs)
        {
            songQueue.push(song);
        }
    }

    // Playlist ko file mein save karna
    void savePlaylist(const string &filename) const
    {
        ofstream outFile(filename);
        if (!outFile)
        {
            throw runtime_error("Unable to open file for writing");
        }
        for (const auto &song : allSongs)
        {
            outFile << song << endl;
        }
        outFile.close();
    }

    // Playlist ko file se load karna
    void loadPlaylist(const string &filename)
    {
        ifstream inFile(filename);
        if (!inFile)
        {
            throw runtime_error("Unable to open file for reading");
        }
        string song;
        allSongs.clear();
        songQueue = queue<string>();
        songIndexMap.clear(); // Clear map

        while (getline(inFile, song))
        {
            allSongs.push_back(song);
            songQueue.push(song);
            songIndexMap[song] = allSongs.size() - 1; // Track song index
        }
        inFile.close();
        n = allSongs.size();
    }

    // Check karna ki queue empty hai ya nahi
    bool isEmpty() const
    {
        return songQueue.empty();
    }

    // Queue ko display karna
    void displayQueue() const
    {
        queue<string> tempQueue = songQueue;
        cout << "Current song queue:" << endl;
        while (!tempQueue.empty())
        {
            cout << tempQueue.front() << endl;
            tempQueue.pop();
        }
    }
};

// Menu options ko display karna
void displayMenu()
{
    cout << "Menu:\n";
    cout << "1. Play next song\n";
    cout << "2. Add a new song\n";
    cout << "3. Remove a song\n";
    cout << "4. Shuffle playlist\n";
    cout << "5. Save playlist\n";
    cout << "6. Load playlist\n";
    cout << "7. Display current song queue\n";
    cout << "8. Exit\n";
    cout << "Enter your choice: ";
}

int main()
{
    int n, k;
    cout << "Enter the number of songs (n): ";
    cin >> n;
    cout << "Enter the repeat interval (k): ";
    cin >> k;

    if (k >= n)
    {
        cerr << "k should be less than n" << endl;
        return 1;
    }

    vector<string> songs;
    string song;
    cout << "Enter the song names (press Enter after each song):" << endl;
    cin.ignore(); // Previous input ka newline ignore karna
    for (int i = 0; i < n; ++i)
    {
        if (!getline(cin, song) || song.empty())
        {
            cerr << "Error: Number of songs provided does not match n" << endl;
            return 1;
        }
        songs.push_back(song);
    }

    SongQueue sq(n, k);
    try
    {
        sq.addSongs(songs);
    }
    catch (const invalid_argument &e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    int choice;
    do
    {
        displayMenu();
        cin >> choice;
        cin.ignore(); // Previous input ka newline ignore karna

        switch (choice)
        {
        case 1:
            try
            {
                cout << "Playing next song: " << sq.getNextSong() << endl;
            }
            catch (const out_of_range &e)
            {
                cerr << e.what() << endl;
            }
            break;
        case 2:
        {
            string newSong;
            cout << "Enter the new song name: ";
            getline(cin, newSong);
            try
            {
                sq.addNewSong(newSong);
                cout << "New song added to the queue." << endl;
            }
            catch (const overflow_error &e)
            {
                cerr << e.what() << endl;
            }
        }
        break;
        case 3:
        {
            string songToRemove;
            cout << "Enter the name of the song to remove: ";
            getline(cin, songToRemove);
            try
            {
                sq.removeSong(songToRemove);
                cout << "Song removed from the queue." << endl;
            }
            catch (const invalid_argument &e)
            {
                cerr << e.what() << endl;
            }
        }
        break;
        case 4:
            sq.shufflePlaylist();
            cout << "Playlist shuffled." << endl;
            break;
        case 5:
        {
            string filename;
            cout << "Enter the filename to save the playlist: ";
            getline(cin, filename);
            try
            {
                sq.savePlaylist(filename);
                cout << "Playlist saved to " << filename << endl;
            }
            catch (const runtime_error &e)
            {
                cerr << e.what() << endl;
            }
        }
        break;
        case 6:
        {
            string filename;
            cout << "Enter the filename to load the playlist: ";
            getline(cin, filename);
            try
            {
                sq.loadPlaylist(filename);
                cout << "Playlist loaded from " << filename << endl;
            }
            catch (const runtime_error &e)
            {
                cerr << e.what() << endl;
            }
        }
        break;
        case 7:
            sq.displayQueue();
            break;
        case 8:
            cout << "Exiting..." << endl;
            break;
        default:
            cerr << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 8);

    return 0;
}
