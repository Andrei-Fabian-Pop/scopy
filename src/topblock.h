#ifndef TOPBLOCK_H
#define TOPBLOCK_H

#include <QObject>
#include <gnuradio/top_block.h>
#include <gnuradio/blocks/copy.h>

class TopBlock : public QObject, public gr::top_block
{
	Q_OBJECT
public:
	TopBlock(std::string name, QObject *parent = nullptr);
	~TopBlock();

	typedef gr::blocks::copy::sptr port_id;
	typedef boost::shared_ptr<basic_block> block;

	void addSource(gr::basic_block_sptr blk);
	void deleteSource(gr::basic_block_sptr toDelete);
	void replaceBlock(gr::basic_block_sptr src, gr::basic_block_sptr dest);

	std::vector<gr::basic_block_sptr> getSources() const;
	unsigned int getNrOfSources() const;

	void lock() {
		gr::top_block::stop();
		gr::top_block::wait();
	}
	void unlock() {
		gr::top_block::start(); }

	/* Set the timeout for the source device */
	void set_device_timeout(unsigned int mseconds);
	void del_connection(gr::basic_block_sptr block, bool reverse);

	/* Connect two regular blocks between themselves. */
	void connect(gr::basic_block_sptr src, int src_port,
		     gr::basic_block_sptr dst, int dst_port);

	/* Disconnect the whole tree of blocks connected to this port ID */
	void disconnect(port_id id);

	/* Disconnect two regular blocks. */
	void disconnect(gr::basic_block_sptr src, int src_port,
			gr::basic_block_sptr dst, int dst_port);


private:
	std::string name;

	bool _started;
	std::vector<gr::basic_block_sptr> sources;

	struct connection {
		gr::basic_block_sptr src;
		gr::basic_block_sptr dst;
		int src_port, dst_port;
	};

	std::vector<connection> connections;

};
#endif // TOPBLOCK_H
