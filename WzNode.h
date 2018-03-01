//////////////////////////////////////////////////////////////////////////////
// This file is part of the PharaohStroy MMORPG client                      // 
// Copyright ?2016-2017 Prime Zeng                                          // 
//                                                                          // 
// This program is free software: you can redistribute it and/or modify     // 
// it under the terms of the GNU Affero General Public License as           // 
// published by the Free Software Foundation, either version 3 of the       // 
// License, or (at your option) any later version.                          // 
//                                                                          // 
// This program is distributed in the hope that it will be useful,          // 
// but WITHOUT ANY WARRANTY; without even the implied warranty of           // 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            // 
// GNU Affero General Public License for more details.                      // 
//                                                                          // 
// You should have received a copy of the GNU Affero General Public License // 
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    // 
////////////////////////////////////////////////////////////////////////////// 

#pragma once
#include <string>
#include <memory>
#include "WzTools.h"
#include "WzReader.h"
#include <unordered_map>
#include <list>
#include <map>
#define WZ_LAZY_LOAD true



class WzNode
{
public:
	typedef std::pair<int32_t, int32_t> vector2i;
	typedef std::map<std::string, WzNode> WzNodes;
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

	WzNode() = default;
	WzNode(const std::shared_ptr<WzReader> &reader);
	WzNode(const std::shared_ptr<WzReader> &reader,WzNode *node);
	~WzNode() = default;
	auto operator==(WzNode const &) const -> bool;
	auto operator!=(WzNode const &) const -> bool;
	auto load(const std::shared_ptr<WzReader> & reader) -> void;
	auto begin()->WzNodes::iterator;
	auto end()->WzNodes::iterator;
	auto operator[](unsigned int)  -> WzNode&;
	auto operator[](signed int) -> WzNode&;
	auto operator[](unsigned long) -> WzNode&;
	auto operator[](signed long) -> WzNode&;
	auto operator[](unsigned long long) -> WzNode&;
	auto operator[](signed long long) -> WzNode&;
	auto operator[](std::string const &) ->WzNode&;
	auto operator[](char const *) ->WzNode&;
	auto operator[](WzNode &) ->WzNode&;

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
	operator WzAudio() ;

	auto getInteger(int64_t defaultValue = 0) const ->int64_t;
	auto getReal(double defaultValue = 0) const -> double;
	auto getString(std::string defaultStr = "")const -> std::string;
	auto getVector(vector2i = { 0,0 }) ->vector2i;
	auto getBitmap() const->WzBitmap;
	auto getAudio() ->WzAudio;
	auto getBoolean(bool def = false) const-> bool;
	auto getChildrenSize()->int32_t;
	auto x() const->int32_t;
	auto y() const->int32_t;
	auto name()->std::string&;
	auto getSize() const->size_t;
	auto root()->WzNode&;
//Takes a '/' separated string, and resolves the given path
	auto resolve(const char* path) -> WzNode&;
	auto resolve(bool expandZero = true) ->WzNode &;
	auto getChild(const char * name) ->WzNode&;
	auto find(const std::string & name)->WzNodes::iterator;
	auto computeEod() -> int32_t;
	auto probeRegion() -> bool;
	auto queryIdentity() -> std::string;
	auto expandRoot(std::string name) -> bool;
	auto tryExpand()->void;
	auto getChildren(bool expand = true)->WzNodes&;
	//尝试展开节点;
	auto childrenSize() -> int;
	auto clear() ->void;
	auto exist(const std::string &name)->bool;
	auto setNodeType(NodeType type) -> void;
	auto getNodeType()->NodeType;
	auto getParent()->WzNode*;
	auto setParent(WzNode * parent) -> void;
	std::string identity;
private:
	auto expandNode(int64_t offset, int64_t eob) ->bool ;
	auto expandShape2dConvex2D(int64_t offset, int64_t eob)   ->bool;
	auto expandShape2dVector2D(int64_t offset) ->bool;
	auto expandSoundDx8(int64_t offset, int64_t eob) ->bool;
	auto expandProperty(int64_t offset) ->bool;
	auto expandCanvas(int64_t offset) ->bool;
	auto expandUol(int64_t offset) ->bool;
	
private:
	std::shared_ptr<WzReader> reader;
	WzNodes children;
	NodeType nodeType = NodeType::NONE;

	int32_t type = 0;
	int32_t size = 0;
	int32_t sum32 = 0;
	int64_t offset = 0;
	Data data;
	int32_t chSize = 0;
	WzNode *parent = nullptr;
};

//More convenience string concatenation operators
auto operator+(std::string, WzNode&)->std::string;
auto operator+(char const *, WzNode&)->std::string;
auto operator+(WzNode&, std::string)->std::string;
auto operator+(WzNode&, char const *)->std::string;
