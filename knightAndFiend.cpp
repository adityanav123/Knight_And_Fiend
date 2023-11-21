/*
    Farmer and the Fiend
 */

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

// global variables
std::mutex globalMutex;
std::condition_variable globalCV;

// Game Status
enum GameStatus { RUNNING, FINISHED };

bool FiendChance = false;

int main() {
  std::vector<int> locations(10, -1);
  int chosenValue = -1;
  int emptyLoc = 10;

  auto _status = GameStatus::RUNNING;

  // create threads
  std::thread fiendThread([&]() {
    while (_status == GameStatus::RUNNING) {
      std::unique_lock<std::mutex> lock(globalMutex);
      if (!FiendChance) {
        globalCV.wait(lock);
      }

      // check if game is over
      if (_status == GameStatus::FINISHED) {
        break;
      }

      std::random_device rd;
      std::mt19937 random(rd());
      std::uniform_int_distribution<> generate(0, 9);

      chosenValue = generate(random);

      if (locations[chosenValue] == -1) {
        emptyLoc = emptyLoc - 1;
        locations[chosenValue] = chosenValue;
      }

      FiendChance = false; // other player chance
      globalCV.notify_one();
    }
  });

  std::thread farmerThread([&]() {
    while (_status == GameStatus::RUNNING) {
      std::unique_lock<std::mutex> lock(globalMutex);

      if (FiendChance) {
        globalCV.wait(lock);
      }
      std::cout << "------------------------------" << std::endl;
    
      if (emptyLoc == 0) {
        std::cout << "no empty locations left!\n";
        _status = GameStatus::FINISHED;
        globalCV.notify_one();
        break;
      }

      // std::cout << "empty locations = " << emptyLoc << std::endl;

      // take user input
      std::cout << "enter location=";
      int idx;
      std::cin >> idx;

      // check invalid chosen value;
      if (idx < 0 || idx > 9) {
        std::cout << "invalid position!\n";
      }
      //  if farmer caught the fiend
      else if (idx == chosenValue) {
        std::cout << "got caught!\n";
        _status = GameStatus::FINISHED;
        globalCV.notify_one();
        break;
      }

      FiendChance = true;
      globalCV.notify_one();
    }
  });

  farmerThread.join();
  fiendThread.join();

  std::cout << "program terminated!" << std::endl;

  return 0;
}
