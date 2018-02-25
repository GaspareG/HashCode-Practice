/*
  License: GPL-3.0
  Author: Gaspare Ferraro <ferraro@gaspa.re>
*/

#include <bits/stdc++.h>
#include <omp.h>

using namespace std;

typedef tuple<int, int, int, int> quad;
typedef tuple<int, int, int> trio;
typedef tuple<int, int> duo;

int R, C, L, H;

char M[1010][1010];

int conta(duo start, duo end, char ing) {
  int x1 = get<0>(start);
  int y1 = get<1>(start);
  int x2 = get<0>(end);
  int y2 = get<1>(end);
  int outs = 0;
  for (int i = y1; i <= y2; i++)
    for (int j = x1; j <= x2; j++)
      if (M[i][j] == ing) outs++;
  return outs;
}

vector<quad> validi;
vector<duo> dimPoss;
priority_queue<trio, vector<trio>, greater<trio>> Q;

int flood(char t, int j, int i) {
  if (M[j][i] == t) return 0;
  int dist = -1;
  for (int d = 1; d < 1000; d++) {
    int miny = max(0, i - d);
    int minx = max(0, j - d);
    int maxy = min(R - 1, i + d);
    int maxx = min(C - 1, j + d);
    if (conta(make_tuple(minx, miny), make_tuple(maxx, maxy), t) >= 1) return d;
  }
  return dist;
}

void trovaSoluzioni() {
  int maxy = -1;
  int maxx = -1;

  if (Q.empty()) return;

  bool trovato = false;
  while (!Q.empty()) {
    trio attuale = Q.top();
    Q.pop();
    maxx = get<1>(attuale);
    maxy = get<2>(attuale);
    if (M[maxy][maxx] != 'X') {
      trovato = true;
      break;
    }
  }

  if (!trovato) return;

  trovato = false;
  for (int i = 0; i < dimPoss.size() && !trovato; i++) {
    int dimx = get<0>(dimPoss[i]);
    int dimy = get<1>(dimPoss[i]);
    for (int ix = 0; ix < dimx && !trovato; ix++)
      for (int iy = 0; iy < dimy && !trovato; iy++) {
        int startx = maxx - ix;
        int starty = maxy - iy;
        int endx = startx + dimx - 1;
        int endy = starty + dimy - 1;
        if (startx < 0 || starty < 0) continue;
        if (endx >= C || endy >= R) continue;
        int tt = conta(duo(startx, starty), duo(endx, endy), 'T');
        int mm = conta(duo(startx, starty), duo(endx, endy), 'M');
        int xx = conta(duo(startx, starty), duo(endx, endy), 'X');
        if (tt >= L && mm >= L && xx == 0) {
          trovato = true;
          for (int py = starty; py <= endy; py++)
            for (int px = startx; px <= endx; px++) M[py][px] = 'X';
          validi.push_back(quad(startx, starty, endx, endy));
        }
      }
  }

  if (trovato && validi.size() % 5000 == 0) {
    Q = priority_queue<trio, vector<trio>, greater<trio>>();
    for (int i = 0; i < R; i++) {
      #pragma omp parallel for schedule(guided)
      for (int j = 0; j < C; j++) {
        if (M[i][j] == 'X') continue;
        int ft = flood('T', i, j);
        int fm = flood('M', i, j);
        #pragma omp critical
        {
          Q.push(trio(ft, j, i));
          Q.push(trio(fm, j, i));
        }
      }
    }
  }
  trovaSoluzioni();
}

void migliora();

int main() {
  scanf("%d %d %d %d", &R, &C, &L, &H);
  for (int i = 0; i < R; i++) scanf("%s", M[i]);

  for (int i = 1; i <= H; i++)
    for (int j = 1; j <= H; j++)
      if (i * j >= 2 * L && i * j <= H) dimPoss.push_back(make_tuple(i, j));

  sort(dimPoss.begin(), dimPoss.end());

  for (int i = 0; i < R; i++)
    for (int j = 0; j < C; j++) {
      Q.push(trio(flood('T', i, j), j, i));
      Q.push(trio(flood('M', i, j), j, i));
    }

  trovaSoluzioni();

  sort(validi.begin(), validi.end());
  for (int i = 0; i < 5000; i++) migliora();

  printf("%d\n",validi.size());
  for (auto att : validi) {
    printf("%d %d %d %d", get<1>(att), get<0>(att), get<3>(att), get<2>(att));
    printf("\n");
  }
}

bool pizza[1000][1000];
void migliora() {
  for (auto att : validi) {
    int x1 = get<0>(att);
    int y1 = get<1>(att);
    int x2 = get<2>(att);
    int y2 = get<3>(att);
    for (int ix = x1; ix <= x2; ix++)
      for (int iy = y1; iy <= y2; iy++) pizza[iy][ix] = true;
  }

  for (int j = 0; j < validi.size(); j++) {
    int x1 = get<0>(validi[j]);
    int y1 = get<1>(validi[j]);
    int x2 = get<2>(validi[j]);
    int y2 = get<3>(validi[j]);
    int fh = y2 - y1 + 1;
    int fw = x2 - x1 + 1;
    bool hMod = false;
    bool wMod = false;
    bool mod = false;

    if ((fh + 1) * fw <= H) hMod = true;
    if (fh * (fw + 1) <= H) wMod = true;

    bool top = false;
    bool bottom = false;
    bool left = false;
    bool right = false;

    if (hMod) {
      if (y1 > 0) {
        bottom = true;
        for (int i = x1; i <= x2; i++)
          if (pizza[y1 - 1][i]) bottom = false;
      }
      if (y2 < R - 1) {
        top = true;
        for (int i = x1; i <= x2; i++)
          if (pizza[y2 + 1][i]) top = false;
      }
    }

    if (wMod) {
      if (x1 > 0) {
        left = true;
        for (int i = y1; i <= y2; i++)
          if (pizza[i][x1 - 1]) left = false;
      }
      if (x2 < C - 1) {
        right = true;
        for (int i = y1; i <= y2; i++)
          if (pizza[i][x2 + 1]) right = false;
      }
    }

    if (bottom) {
      mod = true;
      for (int i = x1; i <= x2; i++) pizza[y1 - 1][i] = true;
      validi[j] = make_tuple(x1, y1 - 1, x2, y2);
    } else if (left) {
      for (int i = y1; i <= y2; i++) pizza[i][x1 - 1] = true;
      validi[j] = make_tuple(x1 - 1, y1, x2, y2);
    } else if (right) {
      for (int i = y1; i <= y2; i++) pizza[i][x2 + 1] = true;
      validi[j] = make_tuple(x1, y1, x2 + 1, y2);
    } else if (top) {
      mod = true;
      for (int i = x1; i <= x2; i++) pizza[y2 + 1][i] = true;
      validi[j] = make_tuple(x1, y1, x2, y2 + 1);
    }
  }
}
