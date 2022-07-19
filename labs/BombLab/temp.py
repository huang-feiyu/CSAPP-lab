if __name__ == "__main__":
    arr = [9, 15, 14, 5, 6, 7]
    for j in range(6):
        print(arr[j], ":")
        for i in range(128):
            if i & 0xf == arr[j]:
                print(chr(i), end="")
        print()

