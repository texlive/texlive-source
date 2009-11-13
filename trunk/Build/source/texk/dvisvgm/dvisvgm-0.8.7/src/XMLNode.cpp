/*************************************************************************
** XMLNode.cpp                                                          **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2009 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        **
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. **
*************************************************************************/

#include "macros.h"
#include "XMLNode.h"
#include "XMLString.h"

using namespace std;

bool XMLNode::emit (ostream &os, XMLNode *stopNode) {
	if (this == stopNode)
		return false;
	write(os);
	return true;
}


XMLElementNode::XMLElementNode (const string &n) : name(n), emitted(false) {
}


XMLElementNode::~XMLElementNode () {
	FORALL(children, ElementList::iterator, i)
		delete *i;
}


void XMLElementNode::addAttribute (const string &name, const string &value) {
	attributes[name] = value;
}


void XMLElementNode::addAttribute (const string &name, double value) {
	attributes[name] = XMLString(value);
}


void XMLElementNode::append (XMLNode *child) {
	if (!child)
		return;
	XMLTextNode *textNode1 = dynamic_cast<XMLTextNode*>(child);
	if (!textNode1 || children.empty())
		children.push_back(child);
	else {
		if (XMLTextNode *textNode2 = dynamic_cast<XMLTextNode*>(children.back()))
			textNode2->append(textNode1);  // merge two consecutive text nodes
		else
			children.push_back(child);
	}
}


void XMLElementNode::append (const string &str) {
	if (children.empty() || !dynamic_cast<XMLTextNode*>(children.back()))
		children.push_back(new XMLTextNode(str));
	else
		static_cast<XMLTextNode*>(children.back())->append(str);
}


void XMLElementNode::prepend (XMLNode *child) {
	if (!child)
		return;
	XMLTextNode *textNode1 = dynamic_cast<XMLTextNode*>(child);
	if (!textNode1 || children.empty())
		children.push_front(child);
	else {
		if (XMLTextNode *textNode2 = dynamic_cast<XMLTextNode*>(children.back()))
			textNode2->prepend(textNode1);  // merge two consecutive text nodes
		else
			children.push_front(child);
	}
}


ostream& XMLElementNode::write (ostream &os) const {
	os << '<' << name;
	FORALL(attributes, AttribMap::const_iterator, i)
		os << ' ' << i->first << "='" << i->second << '\'';
	if (children.empty())
		os << "/>\n";
	else {
		os << '>';
		if (dynamic_cast<XMLElementNode*>(children.front()))
			os << '\n';
		FORALL(children, ElementList::const_iterator, i)
			(*i)->write(os);
		os << "</" << name << ">\n";
	}
	return os;
}


/** Writes a part of the XML tree to the given output stream and removes
 *  the completely written nodes. The output stops when a stop node is reached
 *  (this node won't be printed at all). If a node was only partly emitted, i.e.
 *  its child was the stop node, a further call of emit will continue the output.
 *  @param[in] os stream to which the output is sent to
 *  @param[in] stopElement node where emitting stops (if 0 the whole tree will be emitted)
 *  @return true if node was completely emitted
 * */
bool XMLElementNode::emit (ostream &os, XMLNode *stopNode) {
	if (this == stopNode)
		return false;

	if (!emitted) {
		os << '<' << name;
		FORALL(attributes, AttribMap::iterator, i)
			os << ' ' << i->first << "='" << i->second << '\'';
		if (children.empty())
			os << "/>\n";
		else {
			os << '>';
			if (dynamic_cast<XMLElementNode*>(children.front()))
				os << '\n';
		}

		emitted = true;
	}
	if (!children.empty()) {
		FORALL(children, ElementList::iterator, i) {
			if ((*i)->emit(os, stopNode)) {
				ElementList::iterator it = i++;  // prevent i from being invalidated...
				children.erase(it);              // ... by erase
				--i;  // @@ what happens if i points to first child?
			}
			else
				return false;
		}
		os << "</" << name << ">\n";
	}
	return true;
}


bool XMLElementNode::hasAttribute (const string &name) const {
	return attributes.find(name) != attributes.end();
}


//////////////////////

void XMLTextNode::append (XMLNode *node) {
	if (XMLTextNode *tn = dynamic_cast<XMLTextNode*>(node))
		append(tn);
	else
		delete node;
}


void XMLTextNode::append (XMLTextNode *node) {
	if (node)
		text += node->text;
	delete node;
}


void XMLTextNode::append (const string &str) {
	text += str;
}


void XMLTextNode::prepend (XMLNode *node) {
	if (XMLTextNode *tn = dynamic_cast<XMLTextNode*>(node))
		prepend(tn);
	else
		delete node;
}


//////////////////////

XMLDeclarationNode::XMLDeclarationNode (const string &n, const string &p)
	: name(n), params(p), emitted(false)
{
}


XMLDeclarationNode::~XMLDeclarationNode () {
	FORALL(children, list<XMLDeclarationNode*>::iterator, i)
		delete *i;
}

void XMLDeclarationNode::append (XMLDeclarationNode *child) {
	if (child)
		children.push_back(child);
}

ostream& XMLDeclarationNode::write (ostream &os) const {
	os << "<!" << name << ' ' << params;
	if (children.empty())
		os << ">\n";
	else {
		os << "[\n";
		FORALL(children, list<XMLDeclarationNode*>::const_iterator, i)
			(*i)->write(os);
		os << "]>\n";
	}
	return os;
}


bool XMLDeclarationNode::emit (ostream &os, XMLNode *stopNode) {
	if (this == stopNode)
		return false;

	if (!emitted) {
		os << "<!" << name << ' ' << params;
		if (children.empty())
			os << ">\n";
		else
			os << "[\n";
		emitted = true;
	}
	if (!children.empty()) {
		FORALL(children, list<XMLDeclarationNode*>::iterator, i) {
			if ((*i)->emit(os, stopNode)) {
				list<XMLDeclarationNode*>::iterator it = i++;  // prevent i from being invalidated...
				children.erase(it);              // ... by erase
				--i;  // @@ what happens if i points to first child?
			}
			else
				return false;
		}
		os << "]>\n";
	}
	return true;

}

//////////////////////

ostream& XMLCDataNode::write (ostream &os) const {
	os << "<![CDATA[\n"
		<< data
		<< "]]>\n";
	return os;
}


