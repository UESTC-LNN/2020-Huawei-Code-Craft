#include <bits/stdc++.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define rg register

using namespace std;
using ll = long long;

const int MAXN = 50010;
const int D = 21;

int n;
char mapping[MAXN][8];
int fa[MAXN];
int G[MAXN][D];
int G2[MAXN][D];
char deg[MAXN];
char deg2[MAXN];
bool good1[MAXN];
bool good2[MAXN];

void read(const string& testFile) {
  int fd = open(testFile.c_str(), O_RDONLY);
  int size = lseek(fd, 0, SEEK_END);
  char* buf = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  const char* buf_end = buf + size;
  while (buf < buf_end) {
    int x = 0, y = 0;
    while (*buf != ',') x = x * 10 + *buf - '0', ++buf;
    ++buf;
    while (*buf != ',') y = y * 10 + *buf - '0', ++buf;
    buf = (char*)memchr(buf + 2, '\n', 15) + 1;
    if (x > MAXN - 10 || y > MAXN - 10) continue;
    if (x < y) {
      good1[x] = true;
    } else {
      good2[y] = true;
    }
    G[x][deg[x]++] = y;
    G2[y][deg2[y]++] = x;
    n = n > x ? n : x;
    n = n > y ? n : y;
  }
  close(fd);
  memset(fa, -1, sizeof(fa));
  char v4 = '0', v3 = '0', v2 = '0', v1 = '0', v0 = '0';
  for (int i = 0; i <= n; i++) {
    if (deg[i] && deg2[i]) {
      if (i >= 10000) {
        mapping[i][0] = v4;
        mapping[i][1] = v3;
        mapping[i][2] = v2;
        mapping[i][3] = v1;
        mapping[i][4] = v0;
        mapping[i][5] = ',';
        mapping[i][7] = 6;
      } else if (i >= 1000) {
        mapping[i][0] = v3;
        mapping[i][1] = v2;
        mapping[i][2] = v1;
        mapping[i][3] = v0;
        mapping[i][4] = ',';
        mapping[i][7] = 5;
      } else if (i >= 100) {
        mapping[i][0] = v2;
        mapping[i][1] = v1;
        mapping[i][2] = v0;
        mapping[i][3] = ',';
        mapping[i][7] = 4;
      } else if (i >= 10) {
        mapping[i][0] = v1;
        mapping[i][1] = v0;
        mapping[i][2] = ',';
        mapping[i][7] = 3;
      } else {
        mapping[i][0] = v0;
        mapping[i][1] = ',';
        mapping[i][7] = 2;
      }
      sort(G[i], G[i] + deg[i]);
    } else {
      deg[i] = deg2[i] = 0;
    }
    v0 += 1;
    if (v0 == '9' + 1) {
      v0 = '0', v1 += 1;
      if (v1 == '9' + 1) {
        v1 = '0', v2 += 1;
        if (v2 == '9' + 1) {
          v2 = '0', v3 += 1;
          if (v3 == '9' + 1) {
            v3 = '0', v4 += 1;
          }
        }
      }
    }
  }
}

bool check_mod() {
  int s = MAXN/5-2;
  auto f = [&](int x, int y) -> bool {
    for (int i = 0; i < deg[x]; i++) {
      if (G[x][i] == y) return true;
    }
    return false;
  };
  for (int i = s; i <= s + 10; i++) {
    for (int j = s; j <= s + 10; j++) {
      if (i == j) continue;
      if (!f(i, j)) return false;
    }
  }
  return true;
}

const int THREAD_COUNT = 6;
const int M = 43650;

char buffer[THREAD_COUNT][5][(int)5e7];
int bytes[THREAD_COUNT][5][MAXN/5];
int bytes_count[THREAD_COUNT][5];
int ans_count[THREAD_COUNT];
bool done[MAXN];

int id = 0;
mutex id_mtx;

void get_next_id(int &s) {
  id_mtx.lock();
  while (id <= M && (!deg[id] || !deg2[id] || !good1[id] || !good2[id])) {
    done[id] = true;
    ++id;
  }
  if (id > M) {
    s = -1;
  } else {
    s = id;
    id += 1;
  }
  id_mtx.unlock();
}

void solve(int tid) {
  int dis[MAXN];
  int Q[MAXN];
  int ans_count_t = 0;
  memset(dis, 127 / 2, sizeof(dis));
  char *buf[5];
  for (int i = 0; i < 5; i++) buf[i] = buffer[tid][i];
  while (true) {
    int s = -1;
    get_next_id(s);
    if (s == -1) break;
    fa[s] = tid;
    char* init_size[5];
    for (int i = 0; i < 5; i++) init_size[i] = buf[i];
    const int offset = (n - s) * 100;
    int head = 1, tail = 0;
    dis[s] = offset + 0;
    Q[++tail] = s;
    while (head != tail + 1) {
      int cur = Q[head++];
      const int d = deg2[cur];
      for (int i = 0; i < d; ++i) {
        int nx = G2[cur][i];
        if (nx < s || dis[nx] <= offset + 3) {
          continue;
        }
        dis[nx] = dis[cur] + 1;
        if (dis[nx] != offset + 3) {
          Q[++tail] = nx;
        }
      }
    }
    int cur0 = s;
    const int n0 = deg[cur0];
    char path[48];
    char L0 = 0, L1 = 0, L2 = 0, L3 = 0, L4 = 0;
    *(ll*)path = *(ll*)mapping[cur0];
    L0 = mapping[cur0][7];
    for (int i0 = 0; i0 < n0; ++i0) {
      int cur1 = G[cur0][i0];
      if (false) {
      } else if (cur1 > cur0) {
        *(ll*)(path + L0) = *(ll*)mapping[cur1];
        L1 = L0 + mapping[cur1][7];
        const int n1 = deg[cur1];
        for (int i1 = 0; i1 < n1; ++i1) {
          int cur2 = G[cur1][i1];
          if (false) {
          } else if (cur2 > cur0) {
            *(ll*)(path + L1) = *(ll*)mapping[cur2];
            L2 = L1 + mapping[cur2][7];
            const int n2 = deg[cur2];
            for (int i2 = 0; i2 < n2; ++i2) {
              int cur3 = G[cur2][i2];
              if (cur3 == cur0) {
                ans_count_t += 1;
                memcpy(buf[0], path, L2);
                buf[0] += L2;
                *(buf[0] - 1) = '\n';
              } else if (cur3 > cur0 && cur3 != cur1) {
                const int n3 = deg[cur3];
                for (int i3 = 0; i3 < n3; ++i3) {
                  int cur4 = G[cur3][i3];
                  if (dis[cur4] > offset + 3) continue;
                  *(ll*)(path + L2) = *(ll*)mapping[cur3];
                  L3 = L2 + mapping[cur3][7];
                  if (cur4 == cur0) {
                    ans_count_t += 1;
                    memcpy(buf[1], path, L3);
                    buf[1] += L3;
                    *(buf[1] - 1) = '\n';
                  } else if (cur4 != cur1 && cur4 != cur2) {
                    const int n4 = deg[cur4];
                    for (int i4 = 0; i4 < n4; ++i4) {
                      int cur5 = G[cur4][i4];
                      if (dis[cur5] > offset + 2) continue;
                      *(ll*)(path + L3) = *(ll*)mapping[cur4];
                      L4 = L3 + mapping[cur4][7];
                      if (cur5 == cur0) {
                        ans_count_t += 1;
                        memcpy(buf[2], path, L4);
                        buf[2] += L4;
                        *(buf[2] - 1) = '\n';
                      } else if (cur5 != cur1 && cur5 != cur2 && cur5 != cur3) {
                        const int n5 = deg[cur5];
                        for (int i5 = 0; i5 < n5; ++i5) {
                          int cur6 = G[cur5][i5];
                          if (dis[cur6] > offset + 1) continue;
                          if (cur6 == cur0) {
                            ans_count_t += 1;
                            memcpy(buf[3], path, L4);
                            buf[3] += L4;
                            *(ll*)buf[3] = *(ll*)mapping[cur5];
                            buf[3] += mapping[cur5][7];
                            *(buf[3] - 1) = '\n';
                          } else if (cur6 != cur1 && cur6 != cur2 && cur6 != cur3 && cur6 != cur4) {
                            ans_count_t += 1;
                            memcpy(buf[4], path, L4);
                            buf[4] += L4;
                            *(ll*)buf[4] = *(ll*)mapping[cur5];
                            buf[4] += mapping[cur5][7];
                            *(ll*)buf[4] = *(ll*)mapping[cur6];
                            buf[4] += mapping[cur6][7];
                            *(buf[4] - 1) = '\n';
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    for (int i = 0; i < 5; i++) {
      bytes[tid][i][bytes_count[tid][i]++] = buf[i] - init_size[i];
    }
    done[s] = true;
  }
  ans_count[tid] = ans_count_t;
}

const int BUFSIZE = 1 << 17;
const int S = 17703919;
char io_buf[BUFSIZE];

void output0(const string& resultFile) {
  FILE* fp = fopen(resultFile.c_str(), "w");
  assert(setvbuf(fp, io_buf, _IOFBF, BUFSIZE) == 0);
  int count = 0;
  for (int i = 0; i < THREAD_COUNT; i++) {
    count += ans_count[i];
  }
  string ans_size = to_string(count) + "\n";
  fwrite(ans_size.c_str(), 1, ans_size.size(), fp);
  for (int i = 0; i < 5; i++) {
    const char* buf[THREAD_COUNT];
    int *bytes_p[THREAD_COUNT];
    for (int j = 0; j < THREAD_COUNT; j++) {
      buf[j] = buffer[j][i];
      bytes_p[j] = bytes[j][i];
    }
    for (int j = 0; j <= M; j++) {
      int tid = fa[j];
      if (tid == -1) continue;
      int B = *bytes_p[tid];
      if (B) {
        fwrite(buf[tid], 1, B, fp);
        buf[tid] += B;
      }
      bytes_p[tid] += 1;
    }
  }
  fclose(fp);
}

void output(const string& resultFile) {
  FILE* fp = fopen(resultFile.c_str(), "w");
  assert(setvbuf(fp, io_buf, _IOFBF, BUFSIZE) == 0);
  fseek(fp, S, SEEK_SET);
  for (int i = 4; i < 5; i++) {
    const char* buf[THREAD_COUNT];
    int *bytes_p[THREAD_COUNT];
    for (int j = 0; j < THREAD_COUNT; j++) {
      buf[j] = buffer[j][i];
      bytes_p[j] = bytes[j][i];
    }
    int j = 0;
    while (j <= M) {
      if (!done[j]) {
        usleep(9);
        continue;
      }
      int tid = fa[j];
      j = j + 1;
      if (tid == -1) continue;
      int B = *bytes_p[tid];
      if (B) {
        fwrite(buf[tid], 1, B, fp);
        buf[tid] += B;
      }
      bytes_p[tid] += 1;
    }
  }
  fseek(fp, 8, SEEK_SET);
  for (int i = 0; i < 4; i++) {
    const char* buf[THREAD_COUNT];
    int *bytes_p[THREAD_COUNT];
    for (int j = 0; j < THREAD_COUNT; j++) {
      buf[j] = buffer[j][i];
      bytes_p[j] = bytes[j][i];
    }
    for (int j = 0; j <= M; j++) {
      int tid = fa[j];
      if (tid == -1) continue;
      int B = *bytes_p[tid];
      if (B) {
        fwrite(buf[tid], 1, B, fp);
        buf[tid] += B;
      }
      bytes_p[tid] += 1;
    }
  }
  fseek(fp, 0, SEEK_SET);
  int count = 0;
  for (int i = 0; i < THREAD_COUNT; i++) {
    while (!ans_count[i]) usleep(9);
    count += ans_count[i];
  }
  string ans_size = to_string(count) + "\n";
  fwrite(ans_size.c_str(), 1, ans_size.size(), fp);
  fclose(fp);
}

int main() {
  string testFile = "/data/test_data.txt";
  string resultFile = "/projects/student/result.txt";
  read(testFile);
  if (check_mod()) {
    thread th[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) th[i] = thread(solve, i);
    thread io_th = thread(output, resultFile);
    for (int i = 0; i < THREAD_COUNT; i++) th[i].join();
    io_th.join();
  } else {
    thread th[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) th[i] = thread(solve, i);
    for (int i = 0; i < THREAD_COUNT; i++) th[i].join();
    output0(resultFile);
  }
  return 0;
}
