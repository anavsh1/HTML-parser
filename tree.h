#ifndef tree_H
#define tree_H

/*struct Tag {
  char* type;
  char* text; 
};

struct Tree_Node { 
  char* nodeTextBeforeTags;
  struct Tag tag;
  char* nodeTextAfterTags;
  struct Tree_Node *aChildren[100];
  
}; */

struct Tree_Node {
  char* type;
  //char* content;
  int type_length;
  int tag_type_opening_start_pos;
  int tag_type_opening_end_pos;
  int tag_type_closing_start_pos;
  int tag_type_closing_end_pos;
  int attribute_name_start_pos;
  int attribute_name_end_pos;
  int attribute_value_start_pos;
  int attribute_value_end_pos;
  int iLevel;
  //multiple attributes will need an attribute struct
  struct Tree_Node *aChildren[100];
  bool isAttributePresent;
  bool isValidNode;
  bool isValidTag;
  char* attributeName;
  char* attributeValue;
  char* allContent;
  char* outputText;

};

int addLeaf(struct Tree_Node * parent, struct Tree_Node * leaf);
bool isValidTag(char* tagType);



#endif