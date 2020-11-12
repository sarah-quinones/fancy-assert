# fancy-assert

tries to be a prettier replacement for `assert`. allows for different levels of control.  

[preview on godbolt](https://godbolt.org/z/K5zcad)

interface:

```
// similar to assert(Condition), the DEBUG variant is defined if the NDEBUG macro is not defined
// the ASSERT variant terminates in case of an error, while the EXPECT variant continues exection
FANCY{,DEBUG}_{EXPECT,ASSERT} (Condition)

// same as above, but additionally prints a string to stderr, or executes a callback, depending on the type of the first argument
// The expression producing the callback or message string is only evaluated when the assertion fails
FANCY{,DEBUG}_{EXPECT,ASSERT}_ELSE (Callback_Or_Message_On_Fail, Condition)

// same as above, but checks all the conditions before displaying the error message
FANCY{,DEBUG}_{EXPECT,ASSERT}_ALL_OF (
      (Condition_1),
      ...,
      (Condition_n))

FANCY{,DEBUG}_{EXPECT,ASSERT}_ALL_OF_ELSE (
      (Callback_Or_Message_On_Fail_1, Condition_1),
      ...,
      (Callback_Or_Message_On_Fail_n, Condition_n))
```

e.g., the assertion from `example.cpp`
```cpp
  FANCY_DEBUG_EXPECT_ALL_OF_ELSE(
      ("first assertion", arr[0] == 0), // print message "first assertion"
      ([&] { std::cout << arr[2] << '\n'; }, arr[2] == 1), // call lambda
      ("", arr[1] == 2));                                  // do nothing
```

a possible outcome

![image](https://user-images.githubusercontent.com/40109184/97857527-2bf9e380-1cfe-11eb-8c26-9a06f2c74368.png)
