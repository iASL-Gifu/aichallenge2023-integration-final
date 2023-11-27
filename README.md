# aichallenge2023-final
予選は本レポジトリの最初の数コミットのどれかになります。
決勝はほぼ最新コミットです。



全体的にパラメータはかなり改変しました(特に制御・Planning)。

MPC・PIDにおいては精密なコントロールのためソースコードの変更を少々行っています。

Path-PlanningにおいてはBT-Tree・Paramの変更により起動するモジュールの制限も行いました。
最後のS字はA*とReeds sheppを用いてバック・ドライブの切り返しで乗り越えようとしていました。

RViZにおいては各モジュールの可視化に注力しています。

段ボール検知は途中でCenterPointからEuclidean Clusteringに変更しています。
