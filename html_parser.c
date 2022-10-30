
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "tree.h"


#define STAGE1_INITIAL_TEXT 1
#define STAGE2 200
#define STAGE2_OPENING_TAG 2
#define STAGE2_ATTR_NAME 211
#define STAGE2_ATTR_VALUE 212
#define STAGE3_CAPTURE_CONTENT 3
#define STAGE4 4
#define STAGE5_CLOSING_TAG 5
#define STAGE0_CAPTURE_ONLYTEXT 10
#define STAGE7_CAPTURE_ENTITY 7
#define STAGE8_NESTED_TAG_CAPTURE 8

char finalOutput[5000];
int isRootNode = 0;
int addLeafVal = 0;

bool isWordCountFlagExist;
bool isFrequencyFlagExist;
int inputFileLocation;
int outputFileLocation;
char* frequencyWord;
bool isFormatValid = true;

char* level1[] = {"html"};
char* level2[] = {"title", "body"};
char* level3[] = {"h1", "h2", "h3", "h4", "h5", "h6", "p", "ul", "ol", "section"};
char* level4[] = {"li", "b", "i", "strong", "em", "small", "code", "pre", "sub", "sup", "a", "label", "abbr", "q", "blockquote", "div", "cite", "addr"};

int getLevel (char* tag) {
    int size1 = sizeof(level1)/sizeof(level1[0]);
    int size2 = sizeof(level2)/sizeof(level2[0]);
    int size3 = sizeof(level3)/sizeof(level3[0]);
    int size4 = sizeof(level4)/sizeof(level4[0]);
    int level = 0;
    for (int i = 0; i < size1; i++) {
        if (strcmp(tag, level1[i]) == 0) {
            level = 1;
            return level;
        }
    }

    for (int i = 0; i < size2; i++) {
        if (strcmp(tag, level2[i]) == 0) {
            level = 2;
            return level;
        }
    }

    for (int i = 0; i < size3; i++) {
        if (strcmp(tag, level3[i]) == 0) {
            level = 3;
            return level;
        }
    }

    for (int i = 0; i < size4; i++) {
        if (strcmp(tag, level4[i]) == 0) {
            level = 4;
            return level;
        }
    }

    level = 99; //99 means invalid tag
    return level;
    
} 

char* lowerCase (char* str){
    for(int i = 0; str[i]; i++) {
  		str[i] = tolower(str[i]);
	}
    
    return str;
}

int addLeaf(struct Tree_Node * parent, struct Tree_Node * leaf) {
    addLeafVal++;
    
    printf("addLeaf: %d \n", addLeafVal);
    if (parent != NULL && leaf != NULL) {
        int parentSize = sizeof(parent->aChildren) / sizeof(parent->aChildren[0]);
        for (int i = 0; i < parentSize; i++) {
            if (parent->aChildren[i] == NULL) {
                parent->aChildren[i] = leaf;
                printf("addingLeaf @ i: %d \n", i);
                break;
        } else {
            printf("aChildrenTagType: %s \n", parent->aChildren[i]->type);
        }

    }
        //printf("add leaf %s \n", leaf->tag.type);
        //printf("parentsize: %lu \n", sizeof(parent->aChildren) / sizeof(parent->aChildren[0]) - 1);
        parent->aChildren[sizeof(parent->aChildren) / sizeof(parent->aChildren[0]) - 1] = leaf;
    }
    
    return 0;
}

char *safe_strcpy(char *dest, char *src, size_t size) {
      if (size > 0) {
          size_t i;
          for (i = 0; i < size - 1 && src[i]; i++) {
               dest[i] = src[i];
          }
          dest[i] = '\0';
      }
      return dest;
  }



char *validTags[] = {"html", "body", "h1", "h2", "h3", "h4", "h5", "h6", "p", "li", "ul", "ol", "b", "i", "strong",
 "em", "small", "code", "pre", "sub", "sup", "a", "label", "abbr", "q", "blockquote", "div", "cite", "section"};

bool isValidTag(char* tagType) {

    if (tagType == NULL) return false; 
    int len = sizeof(validTags)/sizeof(validTags[0]);
    for (int i = 0; i < len; ++i) {
        if (strcmp(validTags[i], tagType) == 0) {
            return true;
        }
    }

    return false;
}


void printNode(struct Tree_Node * node) {
    int size = 0;
    printf("tagType: %s \n", node->type);
    printf("isValidTag: %d \n", node->isValidTag);

    printf("tagTypeOpenStartPos: %d \n", node->tag_type_opening_start_pos);
    printf("tagTypeOpenClosePos: %d \n", node->tag_type_opening_end_pos);

    printf("tagTypeCloseStartPos: %d \n", node->tag_type_closing_start_pos);
    printf("tagTypeCloseEndPos: %d \n", node->tag_type_closing_end_pos);

    printf("attributeName: %s \n", node->attributeName);
    printf("attrbuteValue: %s \n", node->attributeValue);

    printf("allContent: %s\n", node->allContent);
    printf("outputText: %s\n", node->outputText);



    for (int i = 0; i < 1000; i++) {
        if (node->aChildren[i] == NULL) {
            size = i;
            break;
        } else {
            printNode(node->aChildren[i]);
            printf("aChildrenTagType: %s \n", node->aChildren[i]->type);
        }

    }

}

  char *ignoreInitialSpace(char* dest, char *src) {
    
    for (int i = 0; i < strlen(src); i++) {
        if (isspace(src[i])) {
            continue;
        } else {
            safe_strcpy(dest, src + i, strlen(src) - i+1);
            break;
            
        }  
    }
    return dest;
  
  }

// it will return true/false value. FOr STAGE 2, it should compare for all STAGE2 variations. 
bool isStage(int iStage, int iCurrentStage) {
    if (iStage == iCurrentStage) {
        return true;
    }
    if (iStage == STAGE2) {
        if (iCurrentStage == STAGE2_ATTR_VALUE){
            return true; 
        } else if (iCurrentStage == STAGE2_ATTR_NAME) {
            return true;
        } else if (iCurrentStage == STAGE2_OPENING_TAG) {
            return true;
        }
    }

    return false;

}

void resetStage(int iCurrentStage){
    iCurrentStage = 0;
}


//char* validEntityNumbers[] = {"&nbsp", "&lt", "&gt", "&amp", "&quot", "&apos", "&cent", "&pound", "&yen", "&euro", "&copy", "&reg", "&sect"};
//char* validEntityNames[] = {"&#160", "&#60", "&#62", "&#38", "&#34", "&#39", "&#162", "&#163", "&#165", "&#8364", "&#169", "&#174", "&#167"};
char* validEntityNumbers[] = {"nbsp", "lt", "gt", "amp", "quot", "apos", "cent", "pound", "yen", "euro", "copy", "reg", "sect"};
char* validEntityNames[] = {"#160", "#60", "#62", "#38", "#34", "#39", "#162", "#163", "#165", "#8364", "#169", "#174", "#167"};
char* replacementEntities[] = {" ", "<", ">", "&", "\"", "\'", "cent", "pound", "yen", "euro", "copyright", "trademark", "section"};
char* replaceEntities(char* text) {
    char* outputText = malloc(5000);
    char* entityText = malloc(10);
    int startEntityPos=0;
    int endEntityPos=0;
    bool isEntityValid = false;
    //int j;
    int size = sizeof(validEntityNames)/sizeof(validEntityNames[0]);
    int eStage = 0; 

    for (int i = 0; i < strlen(text); i++) {
      
        if (eStage != STAGE7_CAPTURE_ENTITY && text[i] == '&') {
            startEntityPos = i;
            eStage = STAGE7_CAPTURE_ENTITY;
            isEntityValid = false; 
        }
        if (eStage == STAGE7_CAPTURE_ENTITY && text[i] == ' '){ // This if should take care of Entity where thre is a space between entity definition. currently this condition will NOT be handled. 
            
            char* incompleteEntityText = malloc(50);
            safe_strcpy(incompleteEntityText, text + startEntityPos , i - startEntityPos +1);
            strcat(outputText, incompleteEntityText);
            eStage = 0; 
        }
        if (eStage == STAGE7_CAPTURE_ENTITY && text[i] == ';'){
            endEntityPos = i; 
            entityText = malloc(50);
            safe_strcpy(entityText, text + startEntityPos + 1, endEntityPos - startEntityPos);
            for (int k = 0; k < size; k++) {
                if (strcmp(entityText, validEntityNumbers[k]) == 0 || strcmp(entityText, validEntityNames[k]) == 0) {
                    entityText = replacementEntities[k];
                    strcat(outputText, entityText);
                    isEntityValid = true; 
                    eStage = 0; 
                    break;
                }   
            }
            if (!isEntityValid){ // dont add it 
                eStage = 0; 
            }
            continue;
        }

        if (eStage == STAGE7_CAPTURE_ENTITY && i == (strlen(text) -1)){ // means no tag found. 
            
            //if (!child_node->isTagValid)){
                //reset the pointer to the position which excludes the tag 
                char cToStr[2] = "\0";
                cToStr[0] = text[i];
                strcat(outputText, cToStr);

                i = startEntityPos + 1;
                eStage = 0; 
                continue;
            //}
        }

        if (eStage != STAGE7_CAPTURE_ENTITY ){
            char cToStr[2] = "\0";
            cToStr[0] = text[i];
            strcat(outputText, cToStr);
        }

    } //end of outer for
    
    return outputText;
}

char* wordArray[10000];
int countArray[10000];
void setWordCount (char* text) {
   char* word;
   int startPos = 0;
   int endPos = 0;
   int i;
   int j = 0;
   bool isWordFound = false;
   char* newText = malloc(5000);
   ignoreInitialSpace(newText, text);
 
   for (i = 0; i < strlen(newText); i++) {
 
   
       //if(newText[i] == ' ' || newText[i] == '\n' ) {
       if (!isalnum(newText[i])){
           endPos = i;
           word = malloc(5000);
           safe_strcpy(word, newText + startPos, endPos - startPos + 1);
           lowerCase(word);
           startPos = i + 1;
           j = 0;
           isWordFound = false;
           while (wordArray[j] != NULL) {
               if (strcmp(word, wordArray[j]) == 0) {
                   countArray[j]++;
                   isWordFound = true;
                   break;
               }
               j++;
           }
 
           if (!isWordFound) {
               wordArray[j] = word;
               countArray[j] = 1;
           }
 
       }
 
   }// end of for
 
   /* printing the array and frequency */
   int k = 0;
   while (wordArray[k] != NULL) {
       printf ("%d -- %s -- %d \n", k, wordArray[k], countArray[k]);
       k++;
   }
 
 
}

int getWordCount() {
   int i = 0;
   int wordCount = 0;
   while (wordArray[i] != NULL) {
       wordCount++;
       i++;
   }
 
   return wordCount;
}
 
int getWordFrequency(char* word) {
   int size = sizeof(wordArray) / sizeof(wordArray[0]);
   for (int i = 0; i < size; i++) {
       if (wordArray[i] != NULL){
           if (strcmp(word, wordArray[i]) == 0) {
           return countArray[i];
           }
       }// end of if.
       else { // end of the loop. Word not found.
           return 0;
       }
      
   }
 
   return 0;
}

void parseArgs(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            isWordCountFlagExist = true;
        }

        if (strcmp(argv[i], "-f") == 0) {
            isFrequencyFlagExist = true;
            frequencyWord = lowerCase(argv[i + 1]);
        }
        inputFileLocation = argc - 2;
        outputFileLocation = argc - 1;
    }
}

//seperate function checkFormat
//traverse parent passed variable
//
//if first child is html, level 1 pass the text
//next child, title body, body can only be 1
//every child should be level 3 only
//under each child should be under level 4

/*
checkFormat(struct Tree_Node * parent) {

} */

int numberOfHtmlTags = 0;
int numberOfBodyTags = 0;
 
bool isFormatCorrect(struct Tree_Node * parent_node, struct Tree_Node * current_node) {
    printf("in isFormatCorrect \n");
    int size = 0;
    int j = 0;
    while (parent_node->aChildren[j] != NULL) {
        size++;
        j++;
    }
    if (parent_node->iLevel == 0) { // root node.
        //then first child of the parent_node should be html.
        if (parent_node->aChildren[0]->iLevel != 1) {
            return false;
        }
    }
       
    if (parent_node->iLevel == 1) { // this is html tag. It can have two children: title (optional) and Body (mandatory) tag.
        if (size > 2) {
            return false;
        }

        for (int i = 0; i < size; i++) {
            if (parent_node->aChildren[i] != NULL) {
                if (getLevel(parent_node->aChildren[i]->type) != 2) {
                    return false;
                }
            }
        }   
    } 
  
   //my parent tag should be one level higher (means lesser number. ) than me.
   if (parent_node->iLevel >= current_node->iLevel) {
       return false;
   }
   
 
   for (int i = 0; i < 1000; i++) {
       if (current_node->aChildren[i] == NULL) {
           break;
       } else {
           if (strcmp(current_node->type, "html") == 0){
               numberOfHtmlTags++;
           }
           if (strcmp(current_node->type, "body") == 0 ){
               numberOfBodyTags++;
           }
          
           if(!isFormatCorrect(current_node, current_node->aChildren[i])) {
               printf("isFormat returning false \n");
               return false;
           } else {
               printf("isFormat returning true \n");
           }
      
       }
 
   }
 
   //check for the number of html and body tags
    if (numberOfHtmlTags != 1 || numberOfBodyTags != 1) {
        return false;
    }

    return true;
 
}

 char* createNode1(struct Tree_Node * parent_node, char* full_node_text) {
    parent_node->outputText = malloc(5000);
    int iCurrentStage = 0; 
    int nested_tag_start_pos = 0;
    int nested_tag_end_pos = 0;

    printf("FullNodeText: %s \n", full_node_text);
    int totalNodeSize = (int) strlen(full_node_text);

    struct Tree_Node * child_node;
    if (totalNodeSize != 0) { // atleast one child and/or text exist.
            child_node = (struct Tree_Node *) malloc (sizeof(struct Tree_Node));
    }

    for (int i = 0; i < totalNodeSize; i++) {
        char currentCharacter = full_node_text[i];
        char nextCharacter;
        if (i == totalNodeSize - 1) {
            nextCharacter = '\0';
        } else {
            nextCharacter = full_node_text[i+1];
        }

        if (!isStage(STAGE3_CAPTURE_CONTENT, iCurrentStage) && currentCharacter == '<') {
            iCurrentStage = STAGE2_OPENING_TAG;
            child_node->tag_type_opening_start_pos = i;
            continue;
        }
        if (isStage(STAGE2_OPENING_TAG, iCurrentStage) && currentCharacter == ' ') {
            
            iCurrentStage = STAGE2_ATTR_NAME;
            child_node->attribute_name_start_pos = i + 1;
            continue;
            
        }
        if (isStage(STAGE2_ATTR_NAME, iCurrentStage) && currentCharacter == '='){
            child_node->attributeName = malloc(5000);
            child_node->attribute_name_end_pos = i;
            safe_strcpy(child_node->attributeName, full_node_text + child_node->attribute_name_start_pos, child_node->attribute_name_end_pos - child_node->attribute_name_start_pos + 1);
            continue;
        }
        if (isStage(STAGE2_ATTR_NAME, iCurrentStage) && currentCharacter == '\"'){
            iCurrentStage = STAGE2_ATTR_VALUE;
            child_node->attribute_value_start_pos = i + 1;
            continue;
        }
        if (isStage(STAGE2_ATTR_VALUE, iCurrentStage) && currentCharacter == '\"'){
            child_node->attributeValue = malloc(5000);
            iCurrentStage = STAGE2_ATTR_VALUE;
            child_node->attribute_value_end_pos = i + 1;
            safe_strcpy(child_node->attributeValue, full_node_text + child_node->attribute_value_start_pos, child_node->attribute_value_end_pos - child_node->attribute_value_start_pos);
            if (strcmp(child_node->attributeName, "title") == 0) {
                strcat(parent_node->outputText, child_node->attributeValue);
                strcat(parent_node->outputText, " ");
            }
            continue;
        }
        if (isStage(STAGE2, iCurrentStage) && currentCharacter == '>') {
            char* typeStr = malloc(50);
            iCurrentStage = STAGE3_CAPTURE_CONTENT;
            child_node->tag_type_opening_end_pos = i;
            
            safe_strcpy(typeStr, full_node_text + child_node->tag_type_opening_start_pos + 1, i - (child_node->tag_type_opening_start_pos));
            child_node->type = lowerCase(typeStr);
            child_node->iLevel = getLevel(child_node->type);
            child_node->isValidTag = isValidTag(child_node->type);
           
            printf("close of openining tag \n");
            continue;
        }

        // nested tag
        if (isStage(STAGE3_CAPTURE_CONTENT, iCurrentStage) && currentCharacter == '<' && nextCharacter != '/') {
            iCurrentStage = STAGE8_NESTED_TAG_CAPTURE;
            nested_tag_start_pos = i; 
            continue;
       
        }
        if (isStage(STAGE8_NESTED_TAG_CAPTURE, iCurrentStage) && currentCharacter == '>') {

            nested_tag_end_pos = i; 

            char* nestedTagType = malloc(50);
           
            safe_strcpy(nestedTagType, full_node_text + nested_tag_start_pos + 1, nested_tag_end_pos - nested_tag_start_pos );
            char* nestedTag_Type = lowerCase(nestedTagType);
            int nestedTag_iLevel = getLevel(nestedTagType);
            bool nestedTag_isValidTag = isValidTag(nestedTagType);
            
            printf("close of nested tag : %s %d %d \n", nestedTag_Type, nestedTag_iLevel, nestedTag_isValidTag);

            if(nestedTag_isValidTag &&  nestedTag_iLevel == child_node->iLevel){
                child_node->type = nestedTag_Type;
                child_node->tag_type_opening_start_pos = nested_tag_start_pos;
                child_node->tag_type_opening_end_pos = nested_tag_end_pos;
            }

            iCurrentStage = STAGE3_CAPTURE_CONTENT;
            continue;
       
        }

        if (isStage(STAGE3_CAPTURE_CONTENT, iCurrentStage) && currentCharacter == '<' && nextCharacter == '/') {
            iCurrentStage = STAGE5_CLOSING_TAG;
            child_node->tag_type_closing_start_pos = i;
            printf("tag close start pos: %d\n ", child_node->tag_type_closing_start_pos);
            //save all content
            continue;

        }
        if (isStage(STAGE5_CLOSING_TAG, iCurrentStage) && currentCharacter == '>'){
            // end tag captured. 
            // if start and end tag match
            char closingTag[10];
            safe_strcpy(closingTag, full_node_text + child_node->tag_type_closing_start_pos + 2, i - (child_node->tag_type_closing_start_pos + 1));
            if (strcmp(child_node->type, lowerCase(closingTag)) == 0) {
                child_node->isValidNode = true;
                child_node->tag_type_closing_end_pos = i;
                char* contentStr = malloc(5000);
                safe_strcpy(contentStr, full_node_text + child_node->tag_type_opening_end_pos + 1, child_node->tag_type_closing_start_pos - (child_node->tag_type_opening_end_pos));
                child_node->allContent = contentStr;
                char* returnStr = createNode1(child_node, child_node->allContent);
                //validate 
                if (child_node->isValidTag) {
                    strcat(parent_node->outputText, returnStr);
                }
                printf("returnString: %s \n", returnStr);
                addLeaf(parent_node, child_node);
                struct Tree_Node * c1_node = (struct Tree_Node *) malloc (sizeof(struct Tree_Node));
                child_node = c1_node;
                iCurrentStage = 0;
                //iCurrentStage = STAGE0_CAPTURE_ONLYTEXT;
                //iCurrentStage = STAGE3_CAPTURE_CONTENT;
            } else {
                iCurrentStage = STAGE3_CAPTURE_CONTENT;
            }
            continue;
        }

        if (iCurrentStage == STAGE3_CAPTURE_CONTENT && i == (totalNodeSize - 1)) { // means no tag found. 
            if (!child_node->isValidTag) {
                //reset the pointer to the position which excludes the tag 
                i = child_node->tag_type_opening_end_pos;
                iCurrentStage =0; 
                continue;
            }

        }
        //if (isStage(STAGE0_CAPTURE_ONLYTEXT, iCurrentStage)) {
        if (iCurrentStage == 0 || isStage(STAGE0_CAPTURE_ONLYTEXT, iCurrentStage)) {
            //captureOnlyText(currentCharacter);
            char cToStr[2] = "\0";
            cToStr[0] = currentCharacter;
            strcat(parent_node->outputText, cToStr);
            //strcat(outputText, cToStr);
        }
        


        printf ("for loop: %d  Char: %c  Stage: %d  \n", i, full_node_text[i], iCurrentStage); 
    }

    /*if (parent_node->type == NULL){
            parent_node->isValidTag = true; 
    }*/
    //******call replace entities here*****
    char* textWithReplacedEntities = replaceEntities(parent_node->outputText);
    return parent_node->outputText =textWithReplacedEntities;  // end of for 
    //return parent_node->outputText ;
 }


int main(int argc, char *argv[]) {
    parseArgs(argc, argv);
    //createNode1()
    char* line;
    long bytes;
    struct Tree_Node * parent = (struct Tree_Node *) malloc (sizeof(struct Tree_Node));
    parent->iLevel = 0;
    FILE * fp;
    printf(" printing args %s %s \n", argv[0], argv[1]);

    //fp = fopen(argv[1], "r");
    fp = fopen(argv[inputFileLocation], "r");
    //fp = fopen("input.html", "r");


    fseek(fp, 0L, SEEK_END);

	bytes = ftell(fp);

	fseek(fp, 0L, SEEK_SET);	
 
	line = (char*)calloc(bytes, sizeof(char));	
 
	fread(line, bytes, 1, fp);
    createNode1(parent, line);
    printf("before print node\n");
    printNode(parent);
    setWordCount(parent->outputText);
    isFormatValid = isFormatCorrect(parent, parent->aChildren[0]);
    if (isFormatValid) {
        if (isWordCountFlagExist) {
            printf("Word Count: %d. ", getWordCount());
        }
        if (isFrequencyFlagExist) {
            printf("%s frequency: %d.", frequencyWord, getWordFrequency(frequencyWord));
        }

    }
	fclose(fp);
    free(line);
    
    //FILE *fptr = fopen(argv[2], "w");
    FILE *fptr = fopen(argv[outputFileLocation], "w");
    if(!isFormatValid) {
        parent->outputText = "";
    }
    fprintf(fptr, "%s", parent->outputText);
    fclose(fptr); 
    return 0;

}
