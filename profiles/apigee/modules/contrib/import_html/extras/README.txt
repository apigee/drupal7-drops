In this folder are some example modules and utilities that have been developed 
for special cases on different real-world sites, though I've tried to make them 
as generic as possible.

These modules should be viewed as examples for how to use the API to add your 
own processing tweaks to the import pipeline.

Hint:
If you are doing an import process that manipulates the $node->body,
you will probably need to also remake the teaser,
eg:
    $node->teaser = node_teaser($node->body, $node->format);

Otherwise the Drupal core teaser calculation will think that your teaser
is deliberately different from the content, and will end up displaying
a possible double-up of the first paragraphs.
This is horrid and frustrating, but is a result of the guesswork done
when deciding if a 'teaser' is to be included in the body.