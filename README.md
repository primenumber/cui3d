CUI3D
====

CUI環境でも3Dアニメーションがしたい！

# ビルド環境

ncurses, pthread, boostとそれなりに新しいC++コンパイラ、cmakeがあれば出来そう

# 動かしてみる

```
$ cmake .
$ make
$ tests/simple01
```

# TODO

- ライブラリとして使えるようにする
- 扱いを簡単に
- 縦横比を何とかする
- 適切な視野
- 視線移動
- 高速化
