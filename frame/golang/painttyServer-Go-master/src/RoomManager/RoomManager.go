package RoomManager

import (
	"Config"
	"Room"
	"Router"
	"Socket"
	"log"
	"net"
	"strconv"
	"sync"
	"time"

	"github.com/syndtr/goleveldb/leveldb"
	"github.com/syndtr/goleveldb/leveldb/opt"
	dbutil "github.com/syndtr/goleveldb/leveldb/util"
	cDebug "github.com/tj/go-debug"
)

var debugOut = cDebug.Debug("RoomManager")

type RoomManager struct {
	ln          *net.TCPListener
	goingClose  chan bool
	router      *Router.Router
	rooms       map[string]*Room.Room
	roomsLocker sync.RWMutex
	db          *leveldb.DB
}

func (m *RoomManager) init() error {
	m.goingClose = make(chan bool)
	m.rooms = make(map[string]*Room.Room)
	m.router = Router.MakeRouter("request")
	m.router.Register("roomlist", m.handleRoomList)
	m.router.Register("newroom", m.handleNewRoom)

	config := Config.GetConfig()

	ideal_port, ok := config["manager_port"].(int)
	if ideal_port <= 0 || !ok {
		log.Println("Manager port is not configured, using default ", ideal_port)
		ideal_port = 18573
	}

	var addr, err = net.ResolveTCPAddr("tcp", ":"+strconv.Itoa(ideal_port))
	if err != nil {
		// handle error
		return err
	}
	m.ln, err = net.ListenTCP("tcp", addr)
	if err != nil {
		// handle error
		return err
	}

	log.Println("RoomManager is listening on port", ideal_port)

	m.recovery()
	go m.shortenRooms()

	return nil
}

func (m *RoomManager) recovery() error {
	db, err := leveldb.OpenFile("./db", nil)
	m.db = db

	iter := db.NewIterator(dbutil.BytesPrefix([]byte("room-")), nil)
	for iter.Next() {
		// Use key/value.
		//key := iter.Key()
		value := iter.Value()
		info, err := parseRoomRuntimeInfo(value)
		if err != nil {
			log.Println("room is corrupted", iter.Key())
			continue
		}
		room, err := Room.RecoverRoom(info)
		if err != nil {
			log.Println("room is corrupted", iter.Key())
			continue
		}
		debugOut("Room recovered", string(value))

		m.roomsLocker.Lock()
		m.rooms[room.Options.Name] = room
		m.roomsLocker.Unlock()
		go func(room *Room.Room, m *RoomManager) {
			roomName := room.Options.Name
			room.Run()
			m.waitRoomClosed(roomName)
		}(room, m)

	}
	iter.Release()
	err = iter.Error()
	return err
}

func (m *RoomManager) shortenRooms() {
	for {
		select {
		case <-time.After(time.Hour):
			iter := m.db.NewIterator(dbutil.BytesPrefix([]byte("room-")), nil)

			batch := new(leveldb.Batch)

			for iter.Next() {
				value := iter.Value()
				info, err := parseRoomRuntimeInfo(value)
				if info.Expiration > 1 && err == nil {
					info.Expiration = info.Expiration - 1

					info_to_insert, err := info.ToJson()
					if err != nil {
						log.Println(err)
						continue
					}
					batch.Put(iter.Key(), info_to_insert)
				} else {
					// delete expired or corrupted room
					batch.Delete(iter.Key())
				}

			}
			iter.Release()
			m.db.Write(batch, nil)
		case _, _ = <-m.goingClose:
			return
		}
	}
}

func (m *RoomManager) waitRoomClosed(roomName string) {
	m.roomsLocker.Lock()
	defer m.roomsLocker.Unlock()
	delete(m.rooms, roomName)
	m.db.Delete([]byte("room-"+roomName), &opt.WriteOptions{false})
}

func (m *RoomManager) Close() {
	m.roomsLocker.Lock()
	defer m.roomsLocker.Unlock()
	close(m.goingClose)
	m.db.Close()
	m.ln.Close()
}

func (m *RoomManager) Run() (err error) {
	err = m.init()
	if err != nil {
		return err
	}
	for {
		select {
		case _, _ = <-m.goingClose:
			return err
		default:
			conn, err := m.ln.AcceptTCP()
			if err != nil {
				// handle error
				continue
			}
			m.processClient(Socket.MakeSocketClient(conn))
		}
	}
	return err
}

//主要业务代码
func (m *RoomManager) processClient(client *Socket.SocketClient) {
	go func() {
		for {
			select {
			case _, _ = <-m.goingClose:
				return
			case pkg, ok := <-client.PackageChan:
				if !ok {
					return
				}
				if pkg.PackageType == Socket.MANAGER {
					err := m.router.OnMessage(pkg.Unpacked, client)
					if err != nil {
						client.Close()
					}
				}
			case _, _ = <-client.GoingClose:
				return
			}
		}
	}()
}

func ServeManager() *RoomManager {
	return &RoomManager{}
}
