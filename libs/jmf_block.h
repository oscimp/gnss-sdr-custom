#ifndef GNSS_SDR_GNSS_SDR_JMF_H
#define GNSS_SDR_GNSS_SDR_JMF_H

#include "concurrent_queue.h"
#include <boost/shared_ptr.hpp>
#include <gnuradio/sync_block.h>  // for sync_block
#include <gnuradio/types.h>       // for gr_vector_const_void_star
#include <pmt/pmt.h>
#include <cstddef>                // for size_t
#include <cstdint>
#include <memory>

class Gnss_JMF;

boost::shared_ptr<Gnss_JMF> gnss_sdr_make_jmf(
    size_t sizeof_stream_item,
    std::shared_ptr<Concurrent_Queue<pmt::pmt_t>> queue);

boost::shared_ptr<Gnss_JMF> gnss_sdr_make_jmf(
    size_t sizeof_stream_item,
    std::shared_ptr<Concurrent_Queue<pmt::pmt_t>> queue);

/*!
 * \brief Implementation of a GNU Radio block that sends a STOP message to the
 * control queue right after a specific number of samples have passed through it.
 */
class Gnss_JMF : public gr::sync_block
{
public:
    int work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items);

private:
    friend boost::shared_ptr<Gnss_JMF> gnss_sdr_make_jmf(
        size_t sizeof_stream_item,
        std::shared_ptr<Concurrent_Queue<pmt::pmt_t>> queue);

    Gnss_JMF(size_t sizeof_stream_item,
        std::shared_ptr<Concurrent_Queue<pmt::pmt_t>> queue);

    uint64_t d_ncopied_items;
    int first_time_;
    std::shared_ptr<Concurrent_Queue<pmt::pmt_t>> d_queue;
};

#endif  // GNSS_SDR_GNSS_SDR_JMF_H
