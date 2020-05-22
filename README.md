# gnss-sdr-custom
GNSS SDR custom processing block

We aim at demonstrating how to add custom frontend processing in 
<a href="https://github.com/gnss-sdr/gnss-sdr">gnss-sdr</a>.

## usage

Copy the files in the subdirectories of <code>src/algorithms/signal_source</code> of
the gnss-sdr source code.

Activate the flag in the configuration file, as shown in UHD-GNSS-SDR-receiver.conf

Compile gnss-sdr and run with <code>gnss-sdr -c UHD-GNSS-SDR-receiver.conf</code>

## why

As opposed to hardware implementations of GNSS receivers, gnss-sdr allows
for controlling all steps of the processing chain, from the reception of the
radiofrequency signal as a stream of I/Q coefficients, to acquisition, tracking
and positioning/timing. Here we are interested in analyzing the properties of
the radiofrequency waves reaching the antenna(s). Since an Ettus Research B210
SDR frontend is used, we start from the UHD source provided by GNSS-SDR, add a 
custom flag for triggering the custom processing block, and implement the block.

Following the GNU Radio architecture, the custom processing block is split between
the block connection flowchart (equivalent to top_block with GNU Radio Companion)
and the processing itself for accessing the I/Q stream.

In the gnss-sdr architecture of Signal Source -> Signal Conditioning -> Acquisition/Tracking,
our aim is to address the initial acquisition step so that the files we are intersted
in are located in src/algorithms/signal_source of the gnss-sdr source code. The
block connection functions are provided in adapters/uhd_signal_source.cc while the
signal processing block is added in libs whose CMakeLists.txt is updated accordingly.

The uhd_signal_source.cc is complemented with an additional flag switched in the configuration
file
``
 jmf_  = configuration->property(role + ".jmf", 0);
``
which, in this implementation, overrides the RF_channels_ variable in the source block, allowing
for a many-to-one stream architecture.

Upon selecting the custom block processing, the object is created
``
 jmf_block_=gnss_sdr_make_jmf(item_size_, queue_);
``
and connected to the UHD source
``
 for (int i = 0; i < RF_channels_; i++)
     {
       if (jmf_ != 0)
          {
             top_block->connect(uhd_source_, i, jmf_block_, i);
             printf("UHD -> JMF connect: %d\n",i);fflush(stdout);
          }
     }
``
Finally, since the signal conditionner will be looking for the right-most
source processing block, the custom block is declared as the last signal source
block:
``
gr::basic_block_sptr UhdSignalSource::get_right_block(int RF_channel)
{
    if ( jmf_ != 0ULL)
        {
            return jmf_block_;
        }
    return uhd_source_;
}
``
which will return the UHD source if the custom block is not selected. The custom
block is now inserted between the UHD source and the signal conditionner.

The implementation of the custom block signal processing follows the classical 
architecture of a GNU Radio block, where a constructor initializes variable and the
work function is provided with the I/Q stream to be processed.

``
int Gnss_JMF::work(int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items)
{   int ch = 0;
    int t;
    unsigned int alignment = volk_get_alignment();
    gr_complex* bufin;
    gr_complex* bufout;
    const gr_complex* in;
    gr_complex* carre=(gr_complex*)volk_malloc(sizeof(gr_complex)*CHUNK_SIZE, alignment);
// see https://github.com/gnss-sdr/gnss-sdr/blob/master/src/algorithms/acquisition/gnuradio_blocks/pcps_acquisition_fine_doppler_cc.h for declaration of gr::fft
    printf("JMF block: %d items, %d out, %d in\n",noutput_items,output_items.size(),input_items.size());
    for (ch = 0; ch < input_items.size(); ch++)
        { // identity: output the same as 1st channel input
          in= (const gr_complex*)input_items[ch]; // all channels
          volk_32fc_x2_multiply_32fc(carre, in, in, CHUNK_SIZE);
          memcpy(output_items[0], carre, noutput_items * input_signature()->sizeof_stream_item(ch));
        }
    volk_free(carre);
    return noutput_items;
}
``
