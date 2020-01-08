package main

import (  
    "fmt"
		"os"
		"time"
)

func main() {
	
    f, err := os.Create("lines.txt")
    if err != nil {
        fmt.Println(err)
                f.Close()
        return
    }
    d := []string{"Welcome to the world of Go1.", "Go is a compiled language.", "It is easy to learn Go."}

    for _, v := range d {
        fmt.Fprintln(f, v)
        if err != nil {
            fmt.Println(err)
            return
        }
    }
    err = f.Close()
    if err != nil {
        fmt.Println(err)
        return
    }
		fmt.Println("file written successfully")
		for true {
			time.Sleep(time.Second)
		}
}
