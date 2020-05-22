# gnss-sdr-custom
GNSS SDR custom processing block

We aim at demonstrating how to add custom frontend processing in gnss-sdr.

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
