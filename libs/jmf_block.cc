/*!
 * inspired from gnss_sdr_valve.cc
 */

#include "jmf_block.h"
#include "command_event.h"
#include <glog/logging.h>           // for LOG
#include <gnuradio/io_signature.h>  // for io_signature
#include <algorithm>                // for min
#include <cstring>                  // for memcpy
#include <unistd.h>                 // for usleep
#include <utility>
#include <volk/volk.h>
#define CHUNK_SIZE (2048*8*2) 

#pragma message("JMF compile")

Gnss_JMF::Gnss_JMF(size_t sizeof_stream_item,
    std::shared_ptr<Concurrent_Queue<pmt::pmt_t>> queue) : gr::sync_block("jmf_block",
                               gr::io_signature::make(1, 20, sizeof_stream_item),
                               gr::io_signature::make(1, 1, sizeof_stream_item)),
                           d_ncopied_items(0),
                           d_queue(std::move(queue))
{
    printf("JMF custom block\n");
/*
Ron Economos (April 5, 2020 10:58 AM)
To: discuss-gnuradio@gnu.org
I would use set_output_multiple() instead. See my previous e-mail for an 
example.
https://lists.gnu.org/archive/html/discuss-gnuradio/2019-08/msg00188.html
*/
    set_output_multiple(CHUNK_SIZE); // only trigger processing if that amount of samples was accumulated
}


boost::shared_ptr<Gnss_JMF> gnss_sdr_make_jmf(size_t sizeof_stream_item, std::shared_ptr<Concurrent_Queue<pmt::pmt_t>> queue)
{
    boost::shared_ptr<Gnss_JMF> jmf_block_(new Gnss_JMF(sizeof_stream_item, std::move(queue)));
    printf("JMF variable created\n");
    return jmf_block_;
}

int Gnss_JMF::work(int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items)
{   long unsigned int ch = 0;
    unsigned int alignment = volk_get_alignment();
    const gr_complex* in;
    gr_complex* carre=(gr_complex*)volk_malloc(sizeof(gr_complex)*CHUNK_SIZE, alignment);
// see https://github.com/gnss-sdr/gnss-sdr/blob/master/src/algorithms/acquisition/gnuradio_blocks/pcps_acquisition_fine_doppler_cc.h for declaration of gr::fft
    printf("JMF block: %d items, %ld out, %ld in\n",noutput_items,output_items.size(),input_items.size());
    for (ch = 0; ch < input_items.size(); ch++)
        { // identity: output the same as 1st channel input
          in= (const gr_complex*)input_items[ch]; // all channels
          volk_32fc_x2_multiply_32fc(carre, in, in, CHUNK_SIZE);
        }
    volk_free(carre);
    memcpy(output_items[0], input_items[0], noutput_items * input_signature()->sizeof_stream_item(0));
    return noutput_items;
}
