#include "WzNode.h"
#include "WzBitmap.h"
#include "WzAudio.h"
#include "WzReader.h"
#include "../Others/Console.h"
#include <vector>
#include <sstream>
#include "WzTools.h"




WzNode::WzNode(const std::shared_ptr<WzReader> &reader) :nodeType(NodeType::NONE), parent(nullptr)
{
	this->reader = reader;
	chSize = 0;

}



WzNode::WzNode(const std::shared_ptr<WzReader> &reader, WzNode *node) :parent(node)
{
	this->reader = reader;
	chSize = 0;
}

auto WzNode::operator==(WzNode const &) const -> bool
{
	return false;
}


auto WzNode::operator!=(WzNode const &) const -> bool
{
	return false;
}

auto WzNode::operator<(WzNode const &) const -> bool
{
	return false;
}

auto WzNode::operator[](unsigned int n)  -> WzNode&
{
	return operator[](std::to_string(n));
}

auto WzNode::operator[](signed int n)  -> WzNode&
{
	return operator[](std::to_string(n));
}

auto WzNode::operator[](unsigned long n)  -> WzNode&
{
	return operator[](std::to_string(n));
}

auto WzNode::operator[](signed long n)  -> WzNode&
{
	return operator[](std::to_string(n));
}

auto WzNode::operator[](unsigned long long n)  -> WzNode&
{
	return operator[](std::to_string(n));
}

auto WzNode::operator[](signed long long n)  -> WzNode&
{
	return operator[](std::to_string(n));
}

auto WzNode::operator[](std::string const &name)  -> WzNode&
{
	return operator[](name.c_str());
}
auto WzNode::operator[](WzNode & node)  -> WzNode&
{
	return operator[](node.getString());
}

auto WzNode::operator[](char const * name)  -> WzNode&
{
	WzNode & node = getChild(name);
	if (node.reader.get() == nullptr) {
		node.reader = reader;
	}

	return node;
}



auto WzNode::getInteger(int64_t defaultValue) const -> int64_t
{
	if (!data.use) {
		return defaultValue;
	}
	switch (nodeType)
	{
	case WzNode::NodeType::NONE:
		break;
	case WzNode::NodeType::INTEGER:
		return data.ireal;
	case WzNode::NodeType::REAL:
		return static_cast<int64_t>(data.dreal);
	case WzNode::NodeType::STRING:
		return std::stoll(data.str);
	case WzNode::NodeType::VECTOR:
		break;
	case WzNode::NodeType::BITMAP:
		break;
	case WzNode::NodeType::AUDIO:
		break;
	default:
		break;
	}
	return defaultValue;
}

auto WzNode::getReal(double defaultValue) const -> double
{
	if (!data.use) {
		return defaultValue;
	}
	switch (nodeType)
	{
	case WzNode::NodeType::NONE:
		break;
	case WzNode::NodeType::INTEGER:
		return static_cast<double>(data.ireal);
	case WzNode::NodeType::REAL:
		return data.dreal;
	case WzNode::NodeType::STRING:
		return std::stod(data.str);
	case WzNode::NodeType::VECTOR:
		break;
	case WzNode::NodeType::BITMAP:
		break;
	case WzNode::NodeType::AUDIO:
		break;
	default:
		break;
	}
	return 0;
}

auto WzNode::getString(std::string defaultStr) const -> std::string
{
	if (!data.use) {
		return defaultStr;
	}
switch (nodeType)
{
case WzNode::NodeType::INTEGER:
	return std::to_string(data.ireal);
case WzNode::NodeType::REAL:
	return std::to_string(static_cast<int64_t>(data.dreal));
case WzNode::NodeType::STRING:
	return data.str;
}
return defaultStr;
}

auto WzNode::getVector(vector2i defaultV) -> vector2i
{
	return std::make_pair(data.vector[0], data.vector[1]);
}

auto WzNode::getBitmap() const -> WzBitmap
{
	return WzBitmap(data.bitmap.height, data.bitmap.width, data.bitmap.length, data.bitmap.offset, data.bitmap.format, data.bitmap.format2, reader);
}

auto WzNode::getAudio() const -> WzAudio
{
	return WzAudio();
}


auto WzNode::getBoolean(bool def) const -> bool
{
	return (data.use && nodeType == NodeType::INTEGER) ? getInteger() ? true : false : def;
}

auto WzNode::x() const -> int32_t
{
	return data.use && nodeType == NodeType::VECTOR ? data.vector[0] : 0;
}

auto WzNode::y() const -> int32_t
{
	return  data.use  && nodeType == NodeType::VECTOR ? data.vector[1] : 0;
}

auto WzNode::name() -> std::string&
{
	return identity;
}

auto WzNode::getSize() const -> size_t
{
	return size;
}

auto WzNode::root()-> WzNode&
{
	if (reader.get()->valid()) {
		reader->setPosition(reader->header.size + 2);
		reader->header.eod = computeEod();
		reader->setPosition(reader->header.size + 2);//
		expandRoot(identity);
	}
	return *this;
}

auto WzNode::expandRoot(std::string name) -> bool
{
	std::vector<WzNode> nodes;
	for (int count = reader->upackInt(); count > 0; --count)
	{
		std::string identity = "";
		byte type = reader->readByte();
		switch (type)
		{
		case 1:
		case 2:
			identity = reader->readDecryptString(reader->header.size + 1 + reader->readInt());
			break;
		case 3:
		case 4:
			identity = reader->readDecryptString();
			break;
		default:
			return false;
		}
		//下一个节点的位置;
		//本节点是从header.length+2开始读取
		int size = reader->upackInt();
		int sum32 = reader->upackInt();
		int offset = reader->computeOffset();
		WzNode node(reader, this);
		node.identity = identity;
		node.type = type;
		node.size = size;
		node.sum32 = sum32;
		node.offset = offset;
		nodes.emplace_back(node);
		
	}
	for (auto &a : nodes) {
		if (0 != a.type % 2) {//folder
			a.expandRoot(a.identity);
		}
		children.emplace(a.identity, a);//children[a.identity] = a;// std::move(a);
	}
	return true;
}

auto WzNode::expandNode(int64_t offset, int64_t eob)  ->bool
{
	std::string type = reader->transitString(offset);//类型;
	int index = WzTools::getIndexInTypes(type);//类型索引;
	bool ret = false;
	switch (index)
	{
	case 0:  ret = expandShape2dConvex2D(offset, eob); break;
	case 1:  ret = expandShape2dVector2D(offset); break;
	case 2:  ret = expandSoundDx8(offset, eob); break;
	case 3:  ret = expandProperty(offset); break;
	case 4:  ret = expandCanvas(offset); break;
	case 5:  ret = expandUol(offset); break;
	default:
		break;
	}
	return ret;
}

auto WzNode::expandShape2dConvex2D(int64_t offset, int64_t eob) -> bool
{
	int count = reader->upackInt();
	for (int index = 0; index < count; ++index)
		if (!expandNode(offset, eob))
			return false;
	return true;
}

auto WzNode::expandShape2dVector2D(int64_t offset)  -> bool
{
	int x = reader->upackInt();
	int y = reader->upackInt();
	setNodeType(NodeType::VECTOR);
	data.use = true;
	data.vector[0] = x;
	data.vector[1] = y;
	return true;
}

auto WzNode::expandSoundDx8(int64_t offset, int64_t eob)  -> bool
{
	int unknow = reader->readByte();
	int size = reader->upackInt();
	int unknow1 = reader->upackInt();
	reader->setPosition(reader->getPosition() + 51);
	nodeType = NodeType::AUDIO;
	data.use = true;
	data.audio.offset = eob - size;
	data.audio.length = size;
	//prop->data = new WzSound(unknow, size, unknow1, eob - size, reader->readByte(), reader);
	reader->setPosition(eob);
	return true;
}

auto WzNode::expandProperty(int64_t offset)  -> bool
{
	int unknow = reader->readShort();
	int count = reader->upackInt();
	chSize = count;
	for (int index = 0; index < count; index++) {
		std::string identity = reader->transitString(offset);
		int type = reader->readByte();
		int64_t position = reader->getPosition();
		WzNode n(reader, this);
		n.data.use = true;
		switch (type)
		{
		case 0x00:
			n.nodeType = NodeType::NONE;
			n.data.use = false;
			break;
		case 0x02:
		case 0x0b:
			n.nodeType = NodeType::INTEGER;
			n.data.ireal = reader->readShort();
		case 0x03:
		case 0x13:
			n.nodeType = NodeType::INTEGER;
			n.data.ireal = reader->upackInt();
			break;
		case 0x04:
			n.nodeType = NodeType::REAL;
			n.data.dreal = reader->upackFloat();
			break;
		case 0x05:
			n.nodeType = NodeType::REAL;
			n.data.dreal = reader->readDouble();
			break;
		case 0x08:
			n.nodeType = NodeType::STRING;
			n.data.str = reader->transitString(offset);
			break;
		case 0x14:
			n.nodeType = NodeType::INTEGER;
			n.data.ireal = reader->upackLong();
			break;
		case 0x09:
			n.data.use = false;
			//下一块的位置;
			int read = reader->readInt();
			int64_t eob = read + reader->getPosition();
			n.expandNode(offset, eob);
			reader->setPosition(eob);
			break;
		}
		n.identity = identity;
		n.offset = position;
		children.emplace(identity, n);
	}
	return true;
}

auto WzNode::expandCanvas(int64_t offset)  -> bool
{

	byte unknow = reader->readByte();

	if (1 == reader->readByte())
		if (!expandProperty(offset))
			return false;

	int width = reader->upackInt();
	int height = reader->upackInt();
	//这句话 有点问题  不知道是这种表示 还是 reader->readByte()+ reader->upackInt()
	//int format = reader->upackInt() + reader->readByte();
	int format = reader->upackInt();
	int f2 = reader->readByte();

	int reserved = reader->readInt();//没用
	int size = reader->readInt();
	int64_t offset2 = reader->getPosition();
	nodeType = NodeType::BITMAP;
	data.use = true;
	data.bitmap.length = size;
	data.bitmap.offset = offset2;
	data.bitmap.format = format;
	data.bitmap.format2 = f2;
	data.bitmap.width = width;
	data.bitmap.height = height;
	reader->setPosition(reader->getPosition() + size);
	return true;
}

auto WzNode::expandUol(int64_t offset)  -> bool
{
	byte v = reader->readByte();
	std::string str = reader->transitString(offset);
	nodeType = NodeType::UOL;
	data.str = str;
	data.use = true;
	if (parent != nullptr) {
		std::vector<std::string> arrays;
		WzTools::split(str, arrays, "/", true);
		WzNode *n = parent;
		for (auto & str : arrays)
		{
			if (str == "..") {
				n = n->parent;
			}
			else {
				break;
			}
		}
		data.ireal = n->offset;//拿到根节点的偏移量;
	}
	return true;
}

auto WzNode::resolve(const std::string & path) -> WzNode &
{
	int64_t currentOffset = reader->getPosition();
	std::vector<std::string> arrays;
	WzTools::split(path, arrays, "/", true);
	std::vector<std::string> a2;
	for (auto & str : arrays)
	{
		if (str != "..") {
			a2.push_back(str);
		}
	}
	auto n = this;
	for (auto & part : a2) {
		n = &n->getChild(part.c_str());
	}
	reader->setPosition(currentOffset);
	return *n;
}

/*
auto WzNode::resolve(const char * path) -> WzNode &
{
	
}*/

/*
auto WzNode::resolve(WzNode &node) ->void
{
	std::vector<std::string> arrays;
	WzTools::split(node.data.str, arrays, "/", true);
	std::vector<std::string> a2;
	for (auto & str : arrays)
	{
		if (str != "..") {
			a2.push_back(str);
		}
	}
	int64_t currentOffset = reader->getPosition();
	
	WzNode *root = new WzNode(reader);
	WzNode *orin = root;
	root->offset = node.data.ireal;//根节点的偏移;
	reader->setPosition(root->offset);
	int i = 0;
	for (auto & s : a2)
	{
		root = &root->getChild(s.c_str());
	}
	node.nodeType = root->nodeType;
	node.data = root->data;
	node.offset = root->offset;

	delete orin;

	reader->setPosition(currentOffset);

}*/

auto WzNode::resolve() ->void
{
	std::vector<std::string> arrays;
	WzTools::split(data.str, arrays, "/", true);
	std::vector<std::string> a2;
	WzNode * p = this->parent;

	for (auto & str : arrays)
	{
		if (str != "..") {
			a2.push_back(str);
		}
		else {
			p = p->parent;
		}
	}
	//int64_t currentOffset = reader->getPosition();
	WzNode *root = p;
	for (auto & s : a2)
	{
		root = &root->getChild(s.c_str());
	}

	nodeType = root->nodeType;
	data = root->data;
	offset = root->offset;
	parent = root->parent;
	identity = root->identity;
//	chSize = root->chSize;
	type = root->type;
//	size = root->size;
	children = root->children;

	


	reader->setPosition(offset);
}

auto WzNode::setNodeType(NodeType type) -> void
{
	this->nodeType = type;
}

auto WzNode::probeRegion() -> bool
{
	for (auto a : { 'k', 'g', 'x' }) {
		reader->setPosition(reader->header.size + 2);
		reader->setRegion(a);
		std::string str = queryIdentity();
		if (WzTools::endWith(str, ".img")) {
			return true;
		}
	}
	return false;
}

auto WzNode::queryIdentity() -> std::string
{
	int children = 0;
	for (int count = reader->upackInt(); count > 0; --count)
	{
		switch (reader->readByte())
		{
		case 1:
			reader->readDecryptString(reader->header.size + 1 + reader->readInt());
			++children;
			break;
		case 2:
			return reader->readDecryptString(reader->header.size + 1 + reader->readInt());
		case 3:
			reader->readDecryptString();
			++children;
			break;
		case 4:
			return reader->readDecryptString();
		default: return "";
		}
		reader->upackInt();
		reader->upackInt();
		reader->readInt();
	}

	while (0 < children--)
	{
		std::string identity = queryIdentity();
		if (identity != "")
			return identity;
	}
	return "";
}



auto WzNode::getChild(const char * name) -> WzNode&
{
	try
	{
		WzNode & node = children.at(name);
		node.parent = this;
		if (node.nodeType == WzNode::NodeType::UOL) {
			node.resolve();
		}
		return node;
	}
	catch (const std::exception&)
	{
		tryExpand();
	}
	WzNode & node = children[name];
	node.parent = this;
	if(node.nodeType == WzNode::NodeType::UOL){
		node.resolve();
	}
	return node;
}
auto WzNode::find(const std::string & name) ->std::map<std::string, WzNode>::iterator
{
	if (this->children.size() == 0) {
		tryExpand();
	}
	return children.find(name);
}
auto WzNode::tryExpand() ->void
{
	reader->setPosition(offset);
	expandNode(offset, 0);
}

auto WzNode::getChildren(bool expand) ->std::map<std::string, WzNode>&
{
	if (expand) {
		if (this->children.size() == 0) {
			tryExpand();
		}
	}
	return children;
}

auto WzNode::childrenSize() -> int
{
	if (children.size() == 0) {
		tryExpand();
	}
	return this->children.size();
}



auto WzNode::clear() ->void
{
	this->children.clear();
}

auto WzNode::computeEod() -> int
{
	int children = 0;
	for (int count = reader->upackInt(); count > 0; --count)
	{
		switch (reader->readByte())
		{
		case 1: reader->readDecryptString(reader->header.size + 1 + reader->readInt()); ++children; break;
		case 2: reader->readDecryptString(reader->header.size + 1 + reader->readInt()); break;
		case 3: reader->readDecryptString(); ++children; break;
		case 4: reader->readDecryptString(); break;
		default: return 0;
		}
		reader->upackInt();
		reader->upackInt();
		reader->readInt();
	}

	while (0 < children--)
		computeEod();
	return (int)reader->getPosition();
}


std::string operator+(std::string s, WzNode & node)
{
	return s + node.getString();
}

std::string operator+(char const * s, WzNode& node)
{
	return s + node.getString();
}

std::string operator+(WzNode&node, std::string s)
{
	return node.getString() + s;
}

std::string operator+(WzNode &node, char const *s)
{
	return node.getString() + s;
}

auto WzNode::operator()(signed long n) -> WzNode
{
	return operator()(std::to_string(n));
}

auto WzNode::operator()(unsigned long long value) -> WzNode
{
	return operator()(std::to_string(value));
}

auto WzNode::operator()(signed long long value) -> WzNode
{
	return operator()(std::to_string(value));
}

auto WzNode::operator()(std::string const &name) -> WzNode
{
	return operator()(name.c_str());
}
auto WzNode::operator()(WzNode & node) -> WzNode
{
	return operator()(node.identity);
}

WzNode::operator unsigned char() const {
	return static_cast<unsigned char>(getInteger());
}
WzNode::operator signed char() const {
	return static_cast<signed char>(getInteger());
}
WzNode::operator unsigned short() const {
	return static_cast<unsigned short>(getInteger());
}
WzNode::operator signed short() const {
	return static_cast<signed short>(getInteger());
}
WzNode::operator unsigned int() const {
	return static_cast<unsigned int>(getInteger());
}
WzNode::operator signed int() const {
	return static_cast<signed int>(getInteger());
}
WzNode::operator unsigned long() const {
	return static_cast<unsigned long>(getInteger());
}
WzNode::operator signed long() const {
	return static_cast<signed long>(getInteger());
}
WzNode::operator unsigned long long() const {
	return static_cast<unsigned long long>(getInteger());
}
WzNode::operator signed long long() const {
	return static_cast<signed long long>(getInteger());
}
WzNode::operator float() const {
	return static_cast<float>(getReal());
}
WzNode::operator double() const {
	return static_cast<double>(getReal());
}
WzNode::operator long double() const {
	return static_cast<long double>(getReal());
}

WzNode::operator std::string() const
{
	return getString();
}
WzNode::operator WzBitmap() const
{
	return WzBitmap(data.bitmap.height, data.bitmap.width, data.bitmap.length, data.bitmap.offset, data.bitmap.format, data.bitmap.format2, reader);
}
WzNode::operator WzAudio() const
{
	return WzAudio();
}
auto WzNode::operator()(unsigned int n) -> WzNode
{
	return operator()(std::to_string(n));
}

auto WzNode::operator()(signed int n) -> WzNode
{
	return operator()(std::to_string(n));
}

auto WzNode::operator()(unsigned long n) -> WzNode
{
	return operator()(std::to_string(n));
}
auto WzNode::operator()(char const * name) -> WzNode
{
	WzNode & node = getChild(name);
	if (node.reader.get() == nullptr) {
		node.reader = reader;
	}
	
	if (node.nodeType == NodeType::UOL) {
		resolve(node);
	}

	return node;
}

