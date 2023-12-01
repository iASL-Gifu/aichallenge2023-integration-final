# WIP:aichallenge2023-integration-final
2023年に開催された自動運転AIチャレンジ(Integration)のチームiASLのソースコードです。

予選は本レポジトリの最初の数コミットのどれかになります。
決勝はprodブランチです。
<br>
## 決勝を終えて
実環境とシミュレーター環境の違いをつくづく実感した大会でした。<br>
ここまで自己位置推定が狂ってしまうとは考えてもいなかったです。<br>
自分達のコードが試験走行の際に動作するのを見た際には大きなやりがいを感じました。

## コード概要
全体的にパラメータはかなり改変しました(特に制御・Planning)。<br>
MPC・PIDにおいては精密なコントロールのためソースコードの変更を少々行っています。<br?
Path-PlanningにおいてはBT-Tree・Paramの変更により起動するモジュールの制限も行いました。<br>
最後のS字はFreeSpacePlannerを改変し、A*とReeds sheppを用いてバック・ドライブの切り返しで乗り越えようとしていました。<br>
RViZにおいては各モジュールの可視化に注力しています。<br>
段ボール検知は途中でCenterPointからEuclidean Clusteringに変更しています。

### Control
#### PID
最後のS字で切り返しを行う予定であったので、指定されたWaypointに正確に発着できるようにチューニングしました。
StoppingとStoppedの移行判定のソースコードをより柔軟になるよう改変しています。
#### MPC
それぞれの重みを上げすぎるとQP-Solverが解けなくなってしまうので、重み付けは慎重に行いました。
QP-SolverはOSQPと比較してUnconstraint-fastの方が安定して解を算出してくれました。
### Planning
### Behavior Path Planning
各モジュールを管理するBT-Treeを改変し、AvoidanceとLaneFollowingのみ機能するように変更しました。
![image](https://github.com/iASL-Gifu/aichallenge2023-integration-final/assets/99851410/27cde24a-6ee4-4950-96bb-9a06fba993d0)
### FreeSpacePlanner
WIP
### Perception
予選ではCenterPointを用いていましたが本番環境ではCenterPointを用いるためのGPUリソースが十分でなかったため,Euclidean Clusteringに変更しました。
### RViZ
制御・Planningのチューニングをより簡便にするために
Float32MultiArrayStampedPieChartでデータの可視化を行いました。
![img_3669](https://github.com/iASL-Gifu/aichallenge2023-integration-final/assets/99851410/0fd29206-451c-481b-9f9e-1a7948038491)
