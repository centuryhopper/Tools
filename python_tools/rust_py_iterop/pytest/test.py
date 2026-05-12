import edit_distance


print("basic cases")
print(edit_distance.min_distance("horse", "ros"))          # 3
print(edit_distance.min_distance("intention", "execution")) # 5

print("\nempty strings")
print(edit_distance.min_distance("", ""))         # 0
print(edit_distance.min_distance("a", ""))        # 1
print(edit_distance.min_distance("", "abc"))      # 3

print("\nidentical strings")
print(edit_distance.min_distance("abc", "abc"))   # 0
print(edit_distance.min_distance("aaaa", "aaaa")) # 0

print("\nsingle character cases")
print(edit_distance.min_distance("a", "b"))       # 1
print(edit_distance.min_distance("a", "a"))       # 0
print(edit_distance.min_distance("a", "ab"))      # 1
print(edit_distance.min_distance("ab", "a"))      # 1

print("\nrepeated patterns")
print(edit_distance.min_distance("aaaaa", "aaa"))     # 2
print(edit_distance.min_distance("ababab", "bababa")) # 2

print("\ncompletely different")
print(edit_distance.min_distance("abc", "xyz"))       # 3
print(edit_distance.min_distance("kitten", "sitting"))# 3

print("\nlonger words")
print(edit_distance.min_distance("algorithm", "altruistic")) # 6
print(edit_distance.min_distance("distance", "editing"))      # 5

print("\nprefix / suffix cases")
print(edit_distance.min_distance("abcdef", "ab"))     # 4
print(edit_distance.min_distance("ab", "abcdef"))     # 4

print("\none character off")
print(edit_distance.min_distance("abcdef", "abcdeg")) # 1
print(edit_distance.min_distance("abcdef", "xbcdef")) # 1

print("\nwhitespace cases")
print(edit_distance.min_distance("a b c", "abc"))     # varies depending on treatment
print(edit_distance.min_distance("abc", "a b c"))     # same

print("\nunicode / extended chars")
print(edit_distance.min_distance("héllo", "hello"))   # 1
print(edit_distance.min_distance("你好", "你好"))       # 0
print(edit_distance.min_distance("你好", "你们好"))     # 1

print("\nlong repeated stress test")
print(edit_distance.min_distance("a" * 50, "a" * 50)) # 0
print(edit_distance.min_distance("a" * 50, "b" * 50)) # 50
print(edit_distance.min_distance("abc" * 20, "abc"))  # 57