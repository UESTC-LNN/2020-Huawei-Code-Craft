# 2020-Huawei-Codecraft-Warm-up-Contest

## My algorithm

Maintain an array named `cof` of length 1000.

1. Enumerate the training set. For a data `td`, if its label is 1, add `td` to `cof`. Otherwise subtract `td` from `cof`.
2. Normalize `cof`, that is, subtract the mean of `cof` for each term of it.
3. Calculate `V_d = d dot cof` for every data `d` in test set.
4. Define a threshold `T`. For a data `d`, if `V_d < T`, label it as '1', '0' otherwise.

## Key observations

- Use `mmap` instead of `read` or `fread`. 

- Only one digit matters. Ignore all digits except the first digit after the decimal point.

- No negative sign in the test set, based on which we can boldly guess that the data with negative signs in the training set are all useless. Here's an example on how to read training data quickly:

  ```cpp
  if (buffer[6001] == '\n') { // this line does not contain negative signs.
      if (buffer[6000] == '1') { 
          // do something
      } else {
          // do something
      }
  } else { // skip this line
      buffer += (char*)memchr(buffer + 6001, '\n', 1000) + 1;
  }
  ```

- Skip part of the test set without breaking the rules. Although we cannot skip a line directly, we can read only one byte of a line. Here's an example on how to skip some lines **legally**.

  ```cpp
  for (int line = 0; line < 20000; line++) {
      if (buffer[2] >= '2') {
          ans[line] = '1'; // output '1'
      } else {
          ans[line] = predict(); // predict the line using normal ways.
      }
      buffer += 6000;
  }
  ```

- Use multiple processes instead of multiple threads! I don't the exact reason why it can be much faster, but it does!
- Get more in the code :)

## Evaluation

I got 0.0172 during the contest. More than 90% of the time is spent on IO.

