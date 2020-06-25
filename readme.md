Test Converter  moodle3
========================

The program is designed to quickly create Moodle XML files
for importing test questions.

Rules for registration of questions
------------------------------------

\# Section

\@ Subsection

? Question choice or multichoice

\* correct

incorrect

? Question numerical or shortanswer

\* answer

?price%% Question choice or multichoice

\*price%% correct

price%%incorrect

?price%% Question numerical or shortanswer

\*price%% answer

\*price%% answer

? Question matching

\* option1->value1

\* option2->value2

?price%% Question matching

\* option1->value1

\* option2->value2

TODO: clozed question

$ Clozed question intro

? Subquestion choice or multichoice

\* correct

incorrect

? Subquestion numerical or shortanswer

\* answer

? Subquestion matching

\* option1->value1

\* option2->value2

?price%% Question choice or multichoice

\*price%% correct

price%%incorrect

?price%% Question numerical or shortanswer

\*price%% answer

\*price%% answer
 
Bugs
-----

Sometimes the input in the document window stops working. Treats itself with a search.