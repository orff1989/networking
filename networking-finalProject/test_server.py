import unittest
import socket

import server
from Client import Client as testClient
from server import Server as testServer

class ServerTest(unittest.TestCase):

    def setUp(self):
        # ------------------------#
        server_IP = "127.0.0.1"

        server_PORT = 55010
        # ------------------------#
        self.cadd = (server_IP, server_PORT)
        self.my_server = testServer()
        self.my_server.making_server()
        self.client1 = testClient()
        #
        self.soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.soc.connect((server_IP, server_PORT))
        self.client1.connect_to_server(server_IP, server_PORT, 'gil')


    def tearDown(self):
        self.soc.close()
#
#
    def test_make_server(self):
        self.assertEqual(testServer.making_server(self), 'Listening')
#
#     def test_client_listen(self):
#         msg = 'set_msg_all'
#         self.soc.send(msg.encode())
#         testServer.clientListen(self, self.soc, self.cadd)
        #self.assertEqual(output, 'Listening')

    def test_2(self):
        a = testClient.connect_to_server_dir(self,"127.0.0.1", 55010)
        self.soc.send('connect'.encode())
        self.assertEqual(self.soc.recv(1024), 'connect')
        self.soc.send('disconnecetd')
        self.assertEqual(self.soc.recv(1024), 'disconnected')

if __name__ == '__main__':
    unittest.main()
