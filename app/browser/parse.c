// This is a part of "13.2.6 Tree construction" in the HTML spec.
// https://html.spec.whatwg.org/multipage/parsing.html#tree-construction

#include "parse.h"

#include "../liumlib/liumlib.h"
#include "lib.h"
#include "tokenize.h"

void insert_child(Node *child) {
  child->parent = current_node;

  if (current_node->first_child == NULL) {
    current_node->first_child = child;
    current_node->last_child = child;

    current_node = child;
    return;
  }

  Node *previous_last_child = current_node->last_child;
  previous_last_child->next = child;
  child->previous = previous_last_child;

  current_node->last_child = child;
  current_node = child;

  push_stack(child);
}

// https://html.spec.whatwg.org/multipage/dom.html#document
Node *create_document() {
  Node *node = (Node *) malloc(sizeof(Node));
  node->element_type = DOCUMENT;
  node->local_name = NULL;
  node->attributes = NULL;
  node->data = NULL;
  node->first_child = NULL;
  node->last_child = NULL;
  node->previous = NULL;
  node->next = NULL;
  return node;
}

Node *create_element(ElementType element_type, char *local_name) {
  Node *node = (Node *) malloc(sizeof(Node));
  node->element_type = element_type;
  if (local_name)
    node->local_name = local_name;
  node->attributes = NULL;
  node->data = NULL;
  node->first_child = NULL;
  node->last_child = NULL;
  node->previous = NULL;
  node->next = NULL;
  return node;
}

// https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token
Node *create_element_from_token(ElementType element_type, Token *token) {
  Node *node = (Node *) malloc(sizeof(Node));
  node->element_type = element_type;
  // "2. Let local name be the tag name of the token."
  if (token->tag_name)
    node->local_name = token->tag_name;
  if (token->attributes)
    node->attributes = token->attributes;
  if (token->data)
    node->data = token->data;
  node->first_child = NULL;
  node->last_child = NULL;
  node->previous = NULL;
  node->next = NULL;
  return node;
}

void push_stack(Node *node) {
  stack_of_open_elements[stack_index] = node;
  stack_index++;
}

Node *pop_stack() {
  return stack_of_open_elements[--stack_index];
}

void construct_tree() {
  Mode mode = INITIAL;

  Node *document = create_document();
  root_node = document;
  current_node = document;

  stack_index = 0;

  Token *token = first_token;

  while (token) {
    switch (mode) {
      case INITIAL:
      println("1 initial");
        // https://html.spec.whatwg.org/multipage/parsing.html#the-initial-insertion-mode
        mode = BEFORE_HTML;
        break;
      case BEFORE_HTML:
      println("2 before html");
        // https://html.spec.whatwg.org/multipage/parsing.html#the-before-html-insertion-mode
        if (token->type == DOCTYPE) {
          // Parse error. Ignore the token.
          token = token->next;
          break;
        }
        if (token->type == START_TAG && strcmp(token->tag_name, "html") == 0) {
          Node *element = create_element_from_token(HTML, token);
          insert_child(element);
          mode = BEFORE_HEAD;
          token = token->next;
          break;
        }
        if (token->type == END_TAG &&
            (strcmp(token->tag_name, "head") != 0 ||
             strcmp(token->tag_name, "body") != 0 ||
             strcmp(token->tag_name, "html") != 0 ||
             strcmp(token->tag_name, "br") != 0)) {
          // Any other end tag
          // Parse error. Ignore the token.
          token = token->next;
          break;
        }
        // Anything else
        {
          Node *element = create_element(HTML, "html");
          insert_child(element);
        }
        mode = BEFORE_HEAD;
        // Reprocess the token.
        break;
      case BEFORE_HEAD:
      println("3 before head");
        // https://html.spec.whatwg.org/multipage/parsing.html#the-before-head-insertion-mode
        if (token->type == DOCTYPE) {
          // A DOCTYPE token
          // Parse error. Ignore the token.
          break;
        }
        if (token->type == START_TAG && strcmp(token->tag_name, "head") == 0) {
          // A start tag whose tag name is "head"
          Node *element = create_element_from_token(HEAD, token);
          insert_child(element);
          mode = IN_HEAD;
          token = token->next;
          break;
        }
        if (token->type == END_TAG &&
            (strcmp(token->tag_name, "head") != 0 ||
             strcmp(token->tag_name, "body") != 0 ||
             strcmp(token->tag_name, "html") != 0 ||
             strcmp(token->tag_name, "br") != 0)) {
          // Any other end tag
          // Parse error. Ignore the token.
          token = token->next;
          break;
        }
        // Anything else
        {
          Node *element = create_element(HEAD, "head");
          insert_child(element);
        }
        mode = IN_HEAD;
        break;
      case IN_HEAD:
      println("4 in head");
        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inhead
        if (token->type == DOCTYPE) {
          // A DOCTYPE token
          // Parse error. Ignore the token.
          break;
        }
        if (token->type == START_TAG && strcmp(token->tag_name, "html") == 0) {
          // A start tag whose tag name is "html"
          // Process the token using the rules for the "in body" insertion mode.
          mode = IN_BODY;
          token = token->next;
          break;
        }
        if (token->type == START_TAG && strcmp(token->tag_name, "head") == 0) {
          // A start tag whose tag name is "head"
          // Parse error. Ignore the token.
          token = token->next;
          break;
        }
        if (token->type == END_TAG && strcmp(token->tag_name, "head") == 0) {
          // An end tag whose tag name is "head"
          token = token->next;
          mode = AFTER_HEAD;
          break;
        }
        if (token->type == END_TAG &&
            (strcmp(token->tag_name, "body") != 0 ||
            strcmp(token->tag_name, "html") != 0 ||
            strcmp(token->tag_name, "br") != 0)) {
          // Any other end tag
          // Parse error. Ignore the token.
          token = token->next;
          break;
        }
        // Anything else
        mode = AFTER_HEAD;
        // Reprocess the token.
        break;
      case AFTER_HEAD:
      println("5 after head");
        // https://html.spec.whatwg.org/multipage/parsing.html#the-after-head-insertion-mode
        if (token->type == DOCTYPE) {
          // A DOCTYPE token
          // Parse error. Ignore the token.
          break;
        }
        if (token->type == START_TAG && strcmp(token->tag_name, "html") == 0) {
          // A start tag whose tag name is "html"
          // Process the token using the rules for the "in body" insertion mode.
          mode = IN_BODY;
          token = token->next;
          break;
        }
        if (token->type == START_TAG && strcmp(token->tag_name, "body") == 0) {
          // A start tag whose tag name is "body"
          Node *element = create_element_from_token(BODY, token);
          insert_child(element);
          mode = IN_BODY;
          token = token->next;
          break;
        }
        if (token->type == START_TAG && strcmp(token->tag_name, "head") == 0) {
          // A start tag whose tag name is "head"
          // Parse error. Ignore the token.
          token = token->next;
          break;
        }
        if (token->type == END_TAG &&
            (strcmp(token->tag_name, "body") != 0 ||
            strcmp(token->tag_name, "html") != 0 ||
            strcmp(token->tag_name, "br") != 0)) {
          // Any other end tag
          // Parse error. Ignore the token.
          token = token->next;
          break;
        }
        // Anything else
        {
          Node *element = create_element(BODY, "body");
          insert_child(element);
        }
        mode = IN_BODY;
        // Reprocess the token.
        break;
      case IN_BODY:
      println("6 in body");
        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-inbody
        if (token->type == CHAR) {
          Node *element = create_element_from_token(TEXT, token);
          insert_child(element);
          token = token->next;
          break;
        }
        if (token->type == DOCTYPE) {
          // A DOCTYPE token
          // Parse error. Ignore the token.
          break;
        }
        if (token->type == EOF) {
          // An end-of-file token
          // Stop parsing.
          return;
        }
        if (token->type == END_TAG && strcmp(token->tag_name, "body") == 0) {
          // An end tag whose tag name is "body"
          if (pop_stack()->element_type != BODY) {
            // Parse error.
          }
          mode = AFTER_BODY;
          token = token->next;
          break;
        }
        if (token->type == END_TAG && strcmp(token->tag_name, "html") == 0) {
          // An end tag whose tag name is "html"
          mode = AFTER_BODY;
          // Reprocess the token.
          break;
        }
        if (token->type == START_TAG && strcmp(token->tag_name, "ul") == 0) {
          Node *element = create_element_from_token(UL, token);
          insert_child(element);
          token = token->next;
          break;
        }
        if (token->type == START_TAG &&
            (strcmp(token->tag_name, "h1") == 0 ||
            strcmp(token->tag_name, "h2") == 0 ||
            strcmp(token->tag_name, "h3") == 0 ||
            strcmp(token->tag_name, "h4") == 0 ||
            strcmp(token->tag_name, "h5") == 0 ||
            strcmp(token->tag_name, "h6") == 0)) {
          // A start tag whose tag name is one of: "h1", "h2", "h3", "h4", "h5", "h6"
          Node *element = create_element_from_token(HEADING, token);
          insert_child(element);
          token = token->next;
          break;
        }
        if (token->type == START_TAG && strcmp(token->tag_name, "li") == 0) {
          // A start tag whose tag name is "li"
          Node *element = create_element_from_token(LI, token);
          insert_child(element);
          token = token->next;
        }
        if (token->type == END_TAG) {
          // Any other end tag
          token = token->next;
          break;
        }
        break;
      case AFTER_BODY:
      println("7 after body");
        // https://html.spec.whatwg.org/multipage/parsing.html#parsing-main-afterbody
        if (token->type == DOCTYPE) {
          // A DOCTYPE token
          // Parse error. Ignore the token.
          break;
        }
        if (token->type == END_TAG && strcmp(token->tag_name, "html") == 0) {
          // An end tag whose tag name is "html"
          token = token->next;
          mode = AFTER_AFTER_BODY;
          break;
        }
        if (token->type == EOF) {
          // An end-of-file token
          // Stop parsing.
          return;
        }
        // Anything else
        // Parse error. Switch the insertion mode to "in body" and reprocess the token.
        mode = IN_BODY;
        break;
      case AFTER_AFTER_BODY:
      println("8 after after body");
        // https://html.spec.whatwg.org/multipage/parsing.html#the-after-after-body-insertion-mode
        if (token->type == EOF) {
          // An end-of-file token
          // Stop parsing.
          return;
        }
        // Anything else
        // Parse error. Switch the insertion mode to "in body" and reprocess the token.
        mode = IN_BODY;
        break;
    }
  }
}

void print_node(Node *node) {
  switch (node->element_type) {
    case DOCUMENT:
      println("DOCUMENT");
      break;
    case HTML:
      println("HTML");
      break;
    case HEAD:
      println("HEAD");
      break;
    case BODY:
      println("BODY");
      break;
    case TEXT:
      println(node->data);
      break;
    default:
      println(node->local_name);
      break;
  }
}

void print_nodes() {
  Node *node = root_node;

  while (node) {
    print_node(node);

    Node *next = node->next;
    while (next) {
      print_node(next);
      next = next->next;
    }

    node = node->first_child;
  }
}
