////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_HTTP_SERVER_ASYNC_JOB_MANAGER_H
#define ARANGOD_HTTP_SERVER_ASYNC_JOB_MANAGER_H 1

#include "Basics/Common.h"
#include "Basics/ReadWriteLock.h"

namespace arangodb {
class GeneralResponse;

namespace rest {
class AsyncCallbackContext;
class GeneralServerJob;

////////////////////////////////////////////////////////////////////////////////
/// @brief AsyncJobResult
////////////////////////////////////////////////////////////////////////////////

struct AsyncJobResult {
 public:
  typedef enum { JOB_UNDEFINED, JOB_PENDING, JOB_DONE } Status;
  typedef uint64_t IdType;

 public:
  AsyncJobResult();

  AsyncJobResult(IdType jobId, GeneralResponse* response, double stamp,
                 Status status, AsyncCallbackContext* ctx);

  ~AsyncJobResult();

 public:
  IdType _jobId;
  GeneralResponse* _response;
  double _stamp;
  Status _status;
  AsyncCallbackContext* _ctx;
};

////////////////////////////////////////////////////////////////////////////////
/// @brief AsyncJobManager
////////////////////////////////////////////////////////////////////////////////

class AsyncJobManager {
  AsyncJobManager(AsyncJobManager const&) = delete;
  AsyncJobManager& operator=(AsyncJobManager const&) = delete;

 public:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief callback typedef
  //////////////////////////////////////////////////////////////////////////////

  typedef void (*callback_fptr)(std::string&, GeneralResponse*);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief joblist typedef
  //////////////////////////////////////////////////////////////////////////////

  typedef std::unordered_map<AsyncJobResult::IdType, AsyncJobResult> JobList;

 public:
  explicit AsyncJobManager(callback_fptr);

  ~AsyncJobManager();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the result of an async job
  //////////////////////////////////////////////////////////////////////////////

  GeneralResponse* getJobResult(AsyncJobResult::IdType, AsyncJobResult::Status&,
                                bool removeFromList);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief deletes the result of an async job
  //////////////////////////////////////////////////////////////////////////////

  bool deleteJobResult(AsyncJobResult::IdType);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief deletes all results
  //////////////////////////////////////////////////////////////////////////////

  void deleteJobResults();

  //////////////////////////////////////////////////////////////////////////////
  /// @brief deletes expired results
  //////////////////////////////////////////////////////////////////////////////

  void deleteExpiredJobResults(double stamp);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the list of pending jobs
  //////////////////////////////////////////////////////////////////////////////

  std::vector<AsyncJobResult::IdType> pending(size_t maxCount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the list of done jobs
  //////////////////////////////////////////////////////////////////////////////

  std::vector<AsyncJobResult::IdType> done(size_t maxCount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief returns the list of jobs by status
  //////////////////////////////////////////////////////////////////////////////

  std::vector<AsyncJobResult::IdType> byStatus(AsyncJobResult::Status,
                                               size_t maxCount);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief initializes an async job
  //////////////////////////////////////////////////////////////////////////////

  void initAsyncJob(GeneralServerJob*, char const*);

  //////////////////////////////////////////////////////////////////////////////
  /// @brief finishes the execution of an async job
  //////////////////////////////////////////////////////////////////////////////

  void finishAsyncJob(AsyncJobResult::IdType jobId,
                      std::unique_ptr<GeneralResponse>);

 private:
  //////////////////////////////////////////////////////////////////////////////
  /// @brief lock to protect the _jobs map
  //////////////////////////////////////////////////////////////////////////////

  basics::ReadWriteLock _lock;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief list of pending/done async jobs
  //////////////////////////////////////////////////////////////////////////////

  JobList _jobs;

  //////////////////////////////////////////////////////////////////////////////
  /// @brief function pointer for callback registered at initialization
  //////////////////////////////////////////////////////////////////////////////

  callback_fptr _callback;
};
}
}

#endif
