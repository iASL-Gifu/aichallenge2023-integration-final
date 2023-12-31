# Reference

## Description

Reference は他のオブジェクトへの参照を行うプラグインです。
同じオブジェクトを共有したり、複雑な定義を分割して記載する際に役立ちます。
ただし、基本的にこのプラグインを直接ユーザーが記載する必要はありません。
オブジェクトが必要な文脈にて文字列型を指定すると、自動的に参照として解釈されます。
参照時の名前空間はウィジェット・ストリーム・フィルターでそれぞれ独立しています。

## Format

| Name  | Type   | Required | Description                                               |
| ----- | ------ | -------- | --------------------------------------------------------- |
| class | string | yes      | プラグインの固有名称である `relay` を指定します。         |
| refer | string | yes      | 参照したいオブジェクトの `label` と同じ名称を指定します。 |
