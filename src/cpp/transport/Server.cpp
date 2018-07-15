// Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <micrortps/agent/transport/Server.hpp>

namespace eprosima {
namespace micrortps {

bool Server::run()
{
    if (!init())
    {
        return false;
    }

    /* Thread initialization. */
    running_cond_ = true;
    receiver_thread_.reset(new std::thread(std::bind(&Server::receiver_loop, this)));
    sender_thread_.reset(new std::thread(std::bind(&Server::sender_loop, this)));
    processing_thread_.reset(new std::thread(std::bind(&Server::processing_loop, this)));

    return true;
}

void Server::stop()
{
    running_cond_ = false;
    input_scheduler_.stop();
    output_scheduler_.stop();
    if (receiver_thread_ && receiver_thread_->joinable())
    {
        receiver_thread_->join();
    }
    if (sender_thread_ && sender_thread_->joinable())
    {
        sender_thread_->join();
    }
    if (processing_thread_ && processing_thread_->joinable())
    {
        processing_thread_->join();
    }
}

void Server::receiver_loop()
{
    input_scheduler_.run();
    InputPacket input_packet;
    while (running_cond_)
    {
        if (recv_message(input_packet, -1))
        {
            input_scheduler_.push(std::move(input_packet), 0);
        }
    }
}

void Server::sender_loop()
{
    output_scheduler_.run();
    OutputPacket output_packet;
    while (running_cond_)
    {
        if (output_scheduler_.pop(output_packet))
        {
            send_message(output_packet);
        }
    }
}

void Server::processing_loop()
{
    // TODO (julian): add processor.
    InputPacket input_packet;
    while (running_cond_)
    {
        if (input_scheduler_.pop(input_packet))
        {
            // TODO (julian): process input_packet.
        }
    }
}

} // namespace micrortps
} // namespace eprosima
