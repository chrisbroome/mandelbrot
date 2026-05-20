#include "compute_worker.h"

#include "transforms.h"

ComputeWorker::ComputeWorker(const std::vector<sf::Color>& palette_)
  : palette(palette_)
  , worker([this]{ run(); })
{}

ComputeWorker::~ComputeWorker() {
  shutdownRequested.store(true);
  currentJobId.fetch_add(1);
  jobCV.notify_one();
  if (worker.joinable()) worker.join();
}

void ComputeWorker::submit(sf::Rect<long double> view, sf::Vector2u size) {
  {
    std::lock_guard<std::mutex> lk(jobMutex);
    const uint64_t newId = currentJobId.fetch_add(1) + 1;
    pendingJob = Job{view, size, newId};
  }
  jobCV.notify_one();
}

std::optional<Result> ComputeWorker::tryTakeResult() {
  std::lock_guard<std::mutex> lk(resultMutex);
  if (!readyResult.has_value()) return std::nullopt;
  auto out = std::move(readyResult);
  readyResult.reset();
  return out;
}

void ComputeWorker::run() {
  while (true) {
    Job job;
    {
      std::unique_lock<std::mutex> lk(jobMutex);
      jobCV.wait(lk, [&]{ return pendingJob.has_value() || shutdownRequested.load(); });
      if (shutdownRequested.load()) return;
      job = *pendingJob;
      pendingJob.reset();
    }

    std::vector<uint8_t> buffer(static_cast<size_t>(job.size.x) * job.size.y * 4);
    const bool completed = computeMandelbrotPixels(
      buffer.data(), job.size, job.view, palette, job.id, currentJobId
    );

    if (completed) {
      std::lock_guard<std::mutex> lk(resultMutex);
      readyResult = Result{job.id, job.size, std::move(buffer)};
    }
  }
}
