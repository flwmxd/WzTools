#pragma once
#include <string>
#include <memory>
#include "WzTools.h"
#include "WzReader.h"
#include <map>

#define WZ_LAZY_LOAD true
class WzNode
{
public:
	typedef std::pair<int32_t, int32_t> vector2i;

	enum class NodeType {
		NONE = 0,
		INTEGER = 1,
		REAL = 2,
		STRING = 3,
		VECTOR = 4,
		BITMAP = 5,
		AUDIO = 6,
		UOL = 7
	};
	NodeType nodeType;
	WzNode() = default;
	WzNode(const std::shared_ptr<WzReader> &reader);
	WzNode(const std::shared_ptr<WzReader> &reader,WzNode *node);
	~WzNode() = default;
	auto operator==(WzNode const &) const -> bool;
	auto operator!=(WzNode const &) const -> bool;
	auto operator<(WzNode const &) const -> bool;
	auto begin() {
		if (children.size() == 0) {
			tryExpand();
		}
		return children.begin();
	}
	auto end()  {
		return children.end();
	}

	auto operator[](unsigned int)  -> WzNode&;
	auto operator[](signed int) -> WzNode&;
	auto operator[](unsigned long) -> WzNode&;
	auto operator[](signed long) -> WzNode&;
	auto operator[](unsigned long long) -> WzNode&;
	auto operator[](signed long long) -> WzNode&;
	auto operator[](std::string const &) ->WzNode&;
	auto operator[](char const *) ->WzNode&;
	auto operator[](WzNode &) ->WzNode&;


	auto operator()(unsigned int)->WzNode;
	auto operator()(signed int)->WzNode;
	auto operator()(unsigned long)->WzNode;
	auto operator()(signed long)->WzNode;
	auto operator()(unsigned long long)->WzNode;
	auto operator()(signed long long)->WzNode;
	auto operator()(std::string const &)->WzNode;
	auto operator()(char const *)->WzNode;
	auto operator()(WzNode &)->WzNode;


	operator unsigned char() const;
	operator signed char() const;
	operator unsigned short() const;
	operator signed short() const;
	operator unsigned int() const;
	operator signed int() const;
	operator unsigned long() const;
	operator signed long() const;
	operator unsigned long long() const;
	operator signed long long() const;
	operator float() const;
	operator double() const;
	operator long double() const;
	operator std::string() const;

	operator WzBitmap() const;
	operator WzAudio() const;


	auto getInteger(int64_t defaultValue = 0) const ->int64_t;
	auto getReal(double defaultValue = 0) const -> double;
	auto getString(std::string defaultStr = "")const -> std::string;
	auto getVector(vector2i = { 0,0 }) ->vector2i;
	auto getBitmap() const->WzBitmap;
	auto getAudio() const ->WzAudio;
	auto getBoolean(bool def = false) const-> bool;

	auto x() const->int32_t;
	auto y() const->int32_t;
	auto name()->std::string&;
	auto getSize() const->size_t;
	auto root()->WzNode&;
//Takes a '/' separated string, and resolves the given path
	auto resolve(const std::string & path) -> WzNode&;
//	auto resolve(const char * path)->WzNode&;
	auto resolve() ->void;
	auto getChild(const char * name) ->WzNode&;
	auto find(const std::string & name)->std::map<std::string, WzNode>::iterator;
	auto computeEod() -> int;
	auto probeRegion() -> bool;
	auto queryIdentity() -> std::string;
	auto expandRoot(std::string name) -> bool;
	auto tryExpand()->void;
	auto getChildren(bool expand = true)->std::map<std::string, WzNode>&;
	auto childrenSize() -> int;
	auto clear() ->void;
	WzNode *parent;
private:
	auto expandNode(int64_t offset, int64_t eob) ->bool ;
	auto expandShape2dConvex2D(int64_t offset, int64_t eob)   ->bool;
	auto expandShape2dVector2D(int64_t offset) ->bool;
	auto expandSoundDx8(int64_t offset, int64_t eob) ->bool;
	auto expandProperty(int64_t offset) ->bool;
	auto expandCanvas(int64_t offset) ->bool;
	auto expandUol(int64_t offset) ->bool;
	auto setNodeType(NodeType type) -> void;

private:
	std::shared_ptr<WzReader> reader;
	std::map<std::string,WzNode> children;


public:
	std::string identity;
	int type;
	int size;
	int sum32;
	int64_t offset;
	Data data;
	int32_t chSize;

};
//More convenience string concatenation operators
std::string operator+(std::string, WzNode&);
std::string operator+(char const *, WzNode&);
std::string operator+(WzNode&, std::string);
std::string operator+(WzNode&, char const *);
