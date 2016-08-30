/*
 *
 */

package main

import (
	"errors"
	"fmt"
	"log"
)

type Node struct {
	Value string
	Data string
	Left *Node
	Right *Node
}


/*
 *	Insert a new value into the BST at the position determined by the search value
 * 	Return Values:
 *		True: if data was successfully inserted
 *		False: if data was unsuccessfully inserted
 *
 *	Pseudocode
 *		1. If the data was already in the tree then return
 *		2. If the data value is less than the current node’s 
 *			value, and if the left child node is nil, insert a
 *			new left child node. Else call Insert on the left subtree.
 *		3. If the data value is greater than the current node's
 *			value, then do the same for the right subtree
 */
func (n *Node) Insert (value, data string) error {
	if n == nil {
		return errors.New("Cannot insert value into a nil tree")
	}

	switch {
	case value == n.Value:
		return nil

	case value < n.Value:
		if n.Left == nil {
			n.Left = &Node{Value: value, Data: data}
			return nil
		}
		return n.Left.Insert(value, data)

	case value > n.Value:
		if n.Right == nil {
			n.Right = &Node{Value: value, Data: data}
			return nil
		}
		return n.Right.Insert(value, data)
	}

	return nil
}

/*
 *	Find searches for a string
 *	Return Values:
 *		The data associated with the value and true if found
 *		"" and false if not found
 *
 *	Pseudocode
 *		1. If the current node contains the value, return it
 *		2. If the data value is less than the current nodes value, 
 *			call find for left child node
  *		3. else call find for right child node
 */
func (n *Node) Find (s string) (string, bool) {
 	if n == nil {
 		return "", false
 	}

 	switch {
 	case s == n.Value:
 		return n.Data, true
 	case s < n.Value:
 		return n.Left.Find(s)
 	default:
 		return n.Right.Find(s)
 	}
}

/*
 *	findMax returns the maximum element in a subtree
 *	Helper funtion for Delete(), this value replaces the
 *	value to be deleted
 *	Return Values:
 *		Node itself, and parent node
 */
func (n *Node) findMax (parent *Node) (*Node, *Node) {
 	if n.Right == nil {
 		return n, parent
 	}
	return n.Right.findMax(n)
}

/*
 *	replaceNode replaces the parent’s child pointer to 
 *	n with a pointer to the replacement node. parent must 
 *	not be nil.
 */
func (n *Node) replaceNode (parent, replacement *Node) error {
	if n == nil {
		return errors.New("replaceNode() not allowed on a nil node")
	}

	if n == parent.Left {
		parent.Left = replacement
		return nil
	}
	parent.Right = replacement
	return nil
}

/*
 * 	Delete removes an element from the tree. It is an error to 
 *	try deleting an element that does not exist. In order to remove 
 *	an element properly, Delete needs to know the node’s parent node. 
 *	parent must not be nil.
 *	Pseudocode:
 *		1. search the node to be deleted
 *		2. if it has no children, then simply remove it from its parent
 *		3. if the node has one child, replace the node with its child
 *		4. If the node has two children, find the maximum node in left subtree
 *			and replace the node's value & data with the replacement node's 
 *			value and data, then remove the replacement node
 */
func (n *Node) Delete (s string, parent *Node) error {
	if n == nil {
		return errors.New("Value to be deleted does not exist in the tree")
	}

	switch {
	case s < n.Value:
		return n.Left.Delete(s, n)
	case s > n.Value:
		return n.Right.Delete(s, n)
	default:
		if n.Left == nil && n.Right == nil {
			n.replaceNode(parent, nil)
			return nil
		}
		if n.Left == nil {
			n.replaceNode(parent, n.Right)
			return nil
		}
		if n.Right == nil {
			n.replaceNode(parent, n.Left)
			return nil
		}

		replacement, replParent := n.Left.findMax(n)
		n.Value = replacement.Value
		n.Data = replacement.Data

		return replacement.Delete(replacement.Value, replParent)
	}
}

