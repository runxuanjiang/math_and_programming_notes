# Prompt Engineering

Notes based on the [Prompt Engineering Whitepaper](https://www.kaggle.com/whitepaper-prompt-engineering).

## What is Prompt Engineering?

LLMs (large language models) takes sequential text as input and try to predict what the next token should be. The **prompt** is the input provided to the model to generate a response. High-quality prompts cause the LLM to produce more accurate outputs.

## Configuring the LLM

### Output Length
This refers to the number of tokens for the LLM to generate.

* Reducing output length only reduces the number of tokens generated, it doesn't make the LLM try to be more succint. If a shorter output length is specified, the prompt may need to be engineered to expect a shorter output.

### Sampling Controls
LLM's predict the probabilities over the next possible tokens, which are sampled. The sampling process can be configured.

**Temperature**
Controls the degree of randomness in token selection

* Temperature 0 is deterministic - highest probability token is always selected.
* Higher temperature means all tokens become equally likely to be the next predicted token.
* In Gemini, temperature is similar to softmax temperature.
* Higher temperature leads to more "creative" results, while lower is used for prompts with more deterministic outputs (like math).


**Top-K and Top-P**
These are settings that configure what set of tokens are considered for prediction (used in conjunction with temperature). Temperature is applied on the set filtered by the Top-K and Top-P configuration.

* Top-K
    * Selects the top K most likely tokens
* Top-P
    * Selects the top tokens whose cumulative probability does not exceed probability P

**repetition loop bug**
Occurs when LLM gets stuck in a cycle and repeatedly generates the same (filler) word or phrase, causing the response to end in a large amount of filler words. Usually caused by too low or high temperature settings.

* Low temperature causes overly deterministic predictions, that can lead to a loop.
* High temperature causes output to be more random, increasing likelihood that a chosen word leads back to a prior state.


## Prompting Techniques

### General prompting (zero-shot prompting)
When a prompt only provides a description of a task for the LLM (zero-shot refers to zero examples provided).

* example: asking LLM to rank a movie review as positive or negative.

### One-shot & fe-shot
One-shot refers to providing a single example, and few-shot provides multiple examples in the prompt.

* example: ask LLM to rank a movie review as positive or negative, and provide examples of existing reviews with their labels.
* Generally, want to choose examples that are diverse, high-quality, and well-written (mistakes can confuse the model).

### System prompting
Sets the overall context and purpose of the language model as a whole.

* Useful for generating output with specific formatting requirements, such as code or JSON.

### Contextual prompting
Provides background relevant to the current conversation/task.

### Role prompting
Assigns a character or identity for the model to adopt.

* example: giving model role of a teacher before telling it to create a lesson plan

### Step-back prompting
First prompt the model with a general question related to the task at hand. Then feed the output (from the LLM) of that question in as context for the specific task.

This allows the LLM to think more about the broader context of the task and reduce bias.

### Chain of Thought (CoT)
Explicitly asking the model to output intermediate steps (think "step by step and explain why") in the prompt.

* Often works well with few-shot prompting (with examples).
* Works well for off-the-shelf LLM's and makes models more robust across different versions.

### Self-consistency
Uses sampling and majority voting to lead to a more consistent answer. This is used together with chain-of-thought so that the trajectory itself can diverge across different answers, leading to different (and diverse) reasoning paths.

1. Prompt the LLM multiple times with the same prompt (with high temperature and CoT).
2. Choose the most common answer.

### Tree of Thoughts (ToT)
Similar to CoT, but explores multiple reasoning paths instead of just one, simultaneously. Can be thought of as a generalization to CoT.

Useful for complex tasks that require exploration.


### ReAct (reason and act)
Combining natural language with external tools to allow the LLM to perform actions and interact with external API's.

In the prompt, the LLM is given options to "act" - it can request output from a Wikipedia search for example. It then does the following iteration in a loop until it reaches an answer:

* Reason about the problem and generate a plan of action.
* Performs the action (or request for the action to be performed) and the observed result is then sent in the next prompt.

### Automatic Prompt Engineering (APE)
Prompting the model to generate more prompts.

### Code Prompting
LLM's can help with the following when it comes to code:

* Generate code
* Explain code
* Translate code from one language to another
* Debug and review code

### Multimodal prompting
Involves prompting the model with different input formats, like images, audio, etc.


## Best Practices
See whitepaper for best practices and examples.

Some useful ones to note:

* Provide examples (few-shot instead of zero-shot)
* Be specific about the output (i.e., output length, style, format)
* Use variables in prompt to save time, such as "tell me a fact about city: {city}"
* Mix up output labels for classification tasks with few-shot examples.
* Use structured input (JSON) for prompts
* Document prompt attempts (see whitepaper for a template for doing this)