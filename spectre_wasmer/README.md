## 実行方法
wasmerで実行する
```
wasmer run cachetiming.wasm
```
オプションでwasm⇒機械語に使うバックエンドコンパイラの変換ができる
* `--llvm`
* `--cranelift`：デフォルト
* `singlepass`