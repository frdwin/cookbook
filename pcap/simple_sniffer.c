/*
 * Simple sniffer recipe.
 */

#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>

void callback_func(u_char *user, const struct pcap_pkthdr *h,
                   const u_char *bytes);
void print_usage(char *program_name);
void err_n_exit(char *msg);

int main(int argc, char **argv) {

  // Check the number of arguments.
  if (argc != 3) {
    print_usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  char *device = argv[1];
  int n_packets = 0;
  char errbuff[PCAP_ERRBUF_SIZE];
  pcap_t *handle;

  // Initialize pcap and check for error.
  if (pcap_init(PCAP_CHAR_ENC_UTF_8, errbuff) == PCAP_ERROR)
    err_n_exit("error initializing pcap library");

  // Open device and check for error.
  if ((handle = pcap_create(device, errbuff)) == NULL)
    err_n_exit("error opening device");

  // Activate sniffer and check for error.
  int result;
  if ((result = pcap_activate(handle)) < 0) {
    switch (result) {
    case PCAP_ERROR_NO_SUCH_DEVICE:
      printf("error: interface not found.\n");
      break;
    case PCAP_ERROR_PERM_DENIED:
      printf("error: permission denied.\n");
      break;
    case PCAP_ERROR_IFACE_NOT_UP:
      printf("error: interface %s is down.\n", device);
      break;
    default:
      printf("undefined error.\n");
    }
    pcap_close(handle);
  }

  // Compile and apply filter.
  struct bpf_program *bpf_p;
  char *filter_string = argv[2];

  if (pcap_compile(handle, bpf_p, filter_string, 0, PCAP_NETMASK_UNKNOWN) ==
      PCAP_ERROR)
    pcap_perror(handle, "error compiling the filter");

  if (pcap_setfilter(handle, bpf_p) == PCAP_ERROR)
    pcap_perror(handle, "error applying the filter");

  // Start sniffing.
  pcap_loop(handle, n_packets, callback_func, NULL);

  // Close the device.
  pcap_close(handle);

  return EXIT_SUCCESS;
}

// Print the usage and example of the arguments.
void print_usage(char *program_name) {
  printf("Usage: %s interface \"filter\"\n", program_name);
  printf("Example: %s wlan0 \"src 192.168.0.1 and dst port 80\"\n",
         program_name);
}

// Show error and exit.
void err_n_exit(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

// Callback function to print the packets informations on stdout.
void callback_func(u_char *user, const struct pcap_pkthdr *h,
                   const u_char *bytes) {
  return; // TODO!
}
