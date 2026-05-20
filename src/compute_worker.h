#pragma once
#ifndef COMPUTE_WORKER_H
#define COMPUTE_WORKER_H

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

#include "SFML/Graphics.hpp"

struct Job {
  sf::Rect<long double> view;
  sf::Vector2u size;
  uint64_t id;
};

struct Result {
  uint64_t id;
  sf::Vector2u size;
  std::vector<uint8_t> rgba;
};

class ComputeWorker {
public:
  explicit ComputeWorker(const std::vector<sf::Color>& palette);
  ~ComputeWorker();

  ComputeWorker(const ComputeWorker&) = delete;
  ComputeWorker& operator=(const ComputeWorker&) = delete;
  ComputeWorker(ComputeWorker&&) = delete;
  ComputeWorker& operator=(ComputeWorker&&) = delete;

  void submit(sf::Rect<long double> view, sf::Vector2u size);
  std::optional<Result> tryTakeResult();

private:
  void run();

  const std::vector<sf::Color>& palette;

  std::mutex jobMutex;
  std::condition_variable jobCV;
  std::optional<Job> pendingJob;

  std::atomic<uint64_t> currentJobId{0};
  std::atomic<bool> shutdownRequested{false};

  std::mutex resultMutex;
  std::optional<Result> readyResult;

  std::thread worker;
};

#endif
