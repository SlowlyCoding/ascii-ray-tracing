class Window {
public:
  int window_width, window_height;
  Window(int window_width, int window_height) : window_width(window_width), window_height(window_height) {};
  void show_cursor(bool show);
  void fill(char *pixels);
  void display(char *pixels);
};

void Window::show_cursor(bool show) {
  if (show) {
    printf("\033[?25h");
  } else {
    printf("\033[?25l");
  }
}

void Window::fill(char *pixels) {
  for (int y=0; y<window_height; y++) {
    for (int x=0; x<window_width; x++) {
      pixels[y*window_width + x] = ' ';
    }
  }
}

void Window::display(char *pixels) {
  // displaying pixels
  for (int y=0; y<window_height; y++) {
    for (int x=0; x<window_width; x++) {
      std::cout << pixels[y*window_width + x];
    }
    std::cout << "\n";
  }
  // set cursor position to top left corner
  printf("\033[%d;%dH",0,0);
}
