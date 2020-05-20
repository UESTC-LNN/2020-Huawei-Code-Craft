#include <bits/stdc++.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define rg register

using namespace std;
using ll = long long;

const int MAXN = 200010;

template <typename T>
struct Vector {
  T *a;
  int n;
  Vector() {
    a = nullptr;
    n = 0;
  }
  void push_back(T x) {
    if ((n & -n) == n) {
      a = (T*)realloc(a, (n * 2 + 1) * sizeof(T));
    }
    a[n++] = x;
  }
  void sort() {
    if (!n) return;
    std::sort(a, a + n);
  }
  void get_max() {
    if (!n) return;
    for (int i = 0; i < n - 1; i++) {
      if (a[i] > a[n - 1]) swap(a[i], a[n - 1]);
    }
  }
  T max() {
    return n ? a[n - 1] : -1;
  }
  struct Iterator {
    int index;
    Vector& outer;
    Iterator(Vector &o, int i): outer(o), index(i) { }
    void operator++() {
      ++index;
    }
    T& operator*() const {
      return outer.a[index];
    }
    bool operator !=(const Iterator& i) {
      return i.index != index;
    }
  };
  Iterator begin() {
    return Iterator(*this, 0);
  }
  Iterator end() {
    return Iterator(*this, n);
  }
};

int n;
char mapping[MAXN][8];
int fa[MAXN];
int G[MAXN][50];
int G2[MAXN][50];
char deg[MAXN];
char deg2[MAXN];

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
    G[x][deg[x]++] = y;
    G2[y][deg2[y]++] = x;
    n = n > x ? n : x;
    n = n > y ? n : y;
  }
  close(fd);
  memset(fa, -1, (n + 1) * sizeof(int));
  for (int i = 0; i <= n; i++) {
    if (i >= 10) {
      *(ll*)(mapping[i]) = *(ll*)(mapping[i / 10]);
      mapping[i][7] += 1;
      mapping[i][mapping[i][7] - 2] = (char)(i % 10 + '0');
      mapping[i][mapping[i][7] - 1] = ',';
    } else {
      mapping[i][7] = 2;
      mapping[i][0] = (char)(i + '0');
      mapping[i][1] = ',';
    }
    sort(G[i], G[i] + deg[i]);
    if (deg2[i]) {
      int max_v = -1;
      int max_v_p = -1;
      for (int j = 0; j < deg2[i]; j++) {
        if (G2[i][j] > max_v) {
          max_v = G2[i][j];
          max_v_p = j;
        }
      }
      if (max_v_p != deg2[i] - 1) {
        swap(G2[i][max_v_p], G2[i][deg2[i] - 1]);
      }
    }
  }
}

const int THREAD_COUNT = 4;

char buffer[THREAD_COUNT][5][(int)5e7];
Vector<int> bytes[THREAD_COUNT][5];
int ans_count[THREAD_COUNT];

int id = 0;
mutex id_mtx;

void get_next_id(int &s) {
  id_mtx.lock();
  while (id <= n && (!deg[id] || !deg2[id] ||
    G[id][deg[id] - 1] < id || G2[id][deg2[id] - 1] < id)) ++id;
  if (id > n) {
    s = -1;
  } else {
    s = id;
    id += 1;
  }
  id_mtx.unlock();
}

void solve(int tid) {
  int dis[n + 1];
  int Q[n + 1];
  int ans_count_t = 0;
  memset(dis, 127 / 2, (n + 1) * sizeof(int));
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
        if (dis[nx] <= offset + 3 || nx < s) {
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
      bytes[tid][i].push_back(buf[i] - init_size[i]);
    }
  }
  ans_count[tid] = ans_count_t;
}

const int BUFSIZE = 1 << 17;
char io_buf[BUFSIZE];

void output(const string& resultFile) {
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
      bytes_p[j] = bytes[j][i].a;
    }
    for (int j = 0; j <= n; j++) {
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

int main() {
  string testFile = "/data/test_data.txt";
  string resultFile = "/projects/student/result.txt";
  read(testFile);
  thread th[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++) th[i] = thread(solve, i);
  for (int i = 0; i < THREAD_COUNT; i++) th[i].join();
  output(resultFile);
  return 0;
}
