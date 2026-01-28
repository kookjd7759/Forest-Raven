<div align="center">
    <h1>Forest Raven: C++ 기반 탐색형 체스 AI 엔진 및 PyQt5 GUI 통합 시스템</h1>
    <p>국동균</p>
</div>

<div align="center">
    <h1>Forest Raven: A C++-Based Search-Driven Chess AI Engine Integrated with a PyQt5 Graphical User Interface</h1>
    <p>Donggyun Kook</p>
</div>

## 요 약
  본 프로젝트는 체스의 모든 규칙을 소프트웨어로 완전하게 구현하고, 자체 개발한 인공지능(AI) 엔진을 통해 사용자가 직접 AI와 대전할 수 있는 체스 시스템 Forest Raven의 개발을 다룬다. 
  본 시스템은 C++ 기반 탐색형 AI 엔진과 Python(PyQt5) 기반 그래픽 사용자 인터페이스(GUI)가 Inter-Process Communication(IPC)을 통해 실시간으로 상호작용하는 통합 구조를 갖는다. AI 엔진은 Minimax Algorithm과 Alpha–Beta Pruning 탐색 기법을 적용하여 수많은 경우의 수 중 최적의 수를 도출하며, 평가 함수(Evaluation Function)는 기물 가치(Piece Value), 위치 가중치(Position Table), 폰 구조(Pawn Structure), 캐슬링 보너스 등의 요소를 종합적으로 계산한다. 
  GUI는 PyQt5를 사용하여 드래그·드롭, 하이라이팅, 애니메이션 이동 등의 인터랙션을 제공함으로써 사용자가 체스를 직관적으로 즐길 수 있도록 설계하였다. 본 프로젝트의 완성본은 학습용·연구용· 게임용으로 다양하게 활용 가능하며, 체스닷컴(Chess.com) 플랫폼에서의 실험을 통해 13전 10승 1무 2패, Rating 662의 성능을 기록하였다. 향후 강화학습 기반 AI와 자동 대국 인터페이스로 확장함으로써, 본 시스템은 AI 연구 플랫폼으로서의 확장성을 가진다.

<img width="250" alt="image" src="https://github.com/user-attachments/assets/78505ad9-46b6-4570-b753-77035a0e1c26" />
<img width="250" height="602" alt="image" src="https://github.com/user-attachments/assets/90a1f397-f417-488b-9e2d-a2dea51edefa" />
<img width="250" height="604" alt="image" src="https://github.com/user-attachments/assets/34446abc-0aa0-4d76-9208-933342729d14" />


## Performance
[Chess.com](https://www.chess.com/stats/overview/forest-raven)
10 0 rapid Rating : 620

