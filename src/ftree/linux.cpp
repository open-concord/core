#if defined(__linux__) // we just need the kernal, don't care about the flavors
#include "../../inc/ftree.hpp"

#include <sys/inotify.h>
#include <unistd.h>
#include <thread>

#define EVENT_SIZE ( sizeof (struct inotify_event) )
#define EBUFF_LEN  ( 1024 * ( EVENT_SIZE + 16 ) )

void
Watch(int fd, int wd, FileTree* tree) {
  // FIXME the way that we do this is kind of horrible
  try {
    char buff[EBUFF_LEN];
    while(true) {
      std::memset(buff, '\0', EBUFF_LEN);
      ssize_t len = read(wd, buff, EBUFF_LEN);

      if (len < 0) {
        printf("error in watch: %d \n", errno); 
      }
    
      for (unsigned int i = 0; i < len; i++) {
        struct inotify_event *event = ( struct inotify_event * ) &buff[i];
        if (!event->len) continue;
      
        if (event->mask & IN_CREATE) {
          if (event->mask & IN_ISDIR) {
            printf("+directory %s \n", event->name);
          } else {
            tree->apply({std::string{event->name}}); 
          }
        }
        i += EVENT_SIZE + event->len;
      }
    }
    
  } catch (std::exception& e) {
    inotify_rm_watch(fd, wd);
    printf("!exception: %s \n", e.what());
    return; // Tree was most likely closed... 
  }
}

void
FileTree::install(std::string path) {
  
  // create new kernel queue instance (if we haven't already)
  if (!kqfd) kqfd = inotify_init();
  assert(0 < kqfd);

  // install on our kq
  int watch_d = inotify_add_watch(kqfd, path.c_str(), IN_CREATE);
  assert(0 < watch_d);

  // now we wait
  std::thread wthread(&Watch, kqfd, watch_d, this);
  wthread.detach();
}

#endif
