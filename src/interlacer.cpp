#include "interlacer.hpp"

void interlacer::calculate_pixel_exit_angles() {
    
}

void interlacer::listen_for_renderer_socket_and_call_dispatcher() {
    std::cout << "Beginning listen." << std::endl;
    shared_vars::acceptor.accept(shared_vars::socket);
    std::cout << "We accepted something!!!!." << std::endl;
    boost::asio::write(shared_vars::socket, boost::asio::buffer("Hello!"));
    std::cout << "Done writing." << std::endl;
    shared_vars::renderer_ready_dispatcher.emit();
}