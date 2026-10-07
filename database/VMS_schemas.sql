CREATE DATABASE IF NOT EXISTS vms_db
    CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE vms_db;

CREATE TABLE roles (
    id          BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    name        VARCHAR(50) NOT NULL UNIQUE,
    description VARCHAR(255),
    created_at  DATETIME DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB;

CREATE TABLE permissions (
    id          BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    code        VARCHAR(100) NOT NULL UNIQUE,
    description VARCHAR(255)
) ENGINE=InnoDB;

CREATE TABLE role_permissions (
    role_id       BIGINT UNSIGNED NOT NULL,
    permission_id BIGINT UNSIGNED NOT NULL,
    PRIMARY KEY (role_id, permission_id),
    FOREIGN KEY (role_id)       REFERENCES roles(id)       ON DELETE CASCADE,
    FOREIGN KEY (permission_id) REFERENCES permissions(id) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE users (
    id                    BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    username              VARCHAR(100) NOT NULL UNIQUE,
    password_hash         VARCHAR(255) NOT NULL,
    full_name             VARCHAR(150),
    email                 VARCHAR(150) UNIQUE,
    phone                 VARCHAR(50),
    is_active             BOOLEAN NOT NULL DEFAULT TRUE,
    must_change_password  BOOLEAN NOT NULL DEFAULT FALSE,
    failed_login_attempts INT UNSIGNED NOT NULL DEFAULT 0,
    locked_until          DATETIME NULL,
    password_changed_at   DATETIME NULL,
    last_login            DATETIME NULL,
    created_at            DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at            DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB;

CREATE TABLE user_roles (
    user_id BIGINT UNSIGNED NOT NULL,
    role_id BIGINT UNSIGNED NOT NULL,
    PRIMARY KEY (user_id, role_id),
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (role_id) REFERENCES roles(id) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE cameras (
    id                    BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    name                  VARCHAR(150) NOT NULL,
    description           TEXT,
    manufacturer          VARCHAR(100),
    model                 VARCHAR(100),
    serial_number         VARCHAR(150) UNIQUE,
    ip_address            VARCHAR(45),
    mac_address           VARCHAR(17) UNIQUE,
    onvif_port            INT UNSIGNED NULL,
    credentials_encrypted VARBINARY(512) NULL,
    location              VARCHAR(255),
    ptz_capable           BOOLEAN NOT NULL DEFAULT FALSE,
    recording_mode        ENUM('continuous','motion','event','disabled') NOT NULL DEFAULT 'continuous',
    retention_days        INT UNSIGNED NULL,
    status                ENUM('online','offline','error','unknown') NOT NULL DEFAULT 'unknown',
    last_seen_at          DATETIME NULL,
    enabled               BOOLEAN NOT NULL DEFAULT TRUE,
    deleted_at            DATETIME NULL,
    created_at            DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at            DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_cameras_ip (ip_address),
    INDEX idx_cameras_status (status)
) ENGINE=InnoDB;

CREATE TABLE camera_streams (
    id             BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    camera_id      BIGINT UNSIGNED NOT NULL,
    stream_name    VARCHAR(100),
    rtsp_url       VARCHAR(1000) NOT NULL, 
    codec          VARCHAR(50),
    resolution     VARCHAR(50),
    fps            DECIMAL(5,2),
    bitrate        INT UNSIGNED,
    is_main_stream BOOLEAN NOT NULL DEFAULT TRUE,
    enabled        BOOLEAN NOT NULL DEFAULT TRUE,
    FOREIGN KEY (camera_id) REFERENCES cameras(id) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE camera_groups (
    id          BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    name        VARCHAR(100) NOT NULL UNIQUE,
    description TEXT
) ENGINE=InnoDB;

CREATE TABLE camera_group_members (
    group_id  BIGINT UNSIGNED NOT NULL,
    camera_id BIGINT UNSIGNED NOT NULL,
    PRIMARY KEY (group_id, camera_id),
    FOREIGN KEY (group_id)  REFERENCES camera_groups(id) ON DELETE CASCADE,
    FOREIGN KEY (camera_id) REFERENCES cameras(id)       ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE role_camera_groups (
    role_id  BIGINT UNSIGNED NOT NULL,
    group_id BIGINT UNSIGNED NOT NULL,
    PRIMARY KEY (role_id, group_id),
    FOREIGN KEY (role_id)  REFERENCES roles(id)         ON DELETE CASCADE,
    FOREIGN KEY (group_id) REFERENCES camera_groups(id) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE storage_locations (
    id                 BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    name               VARCHAR(100) NOT NULL UNIQUE,
    base_path          VARCHAR(500) NOT NULL,
    max_storage_gb     INT UNSIGNED,
    current_storage_gb DECIMAL(10,2) NOT NULL DEFAULT 0,
    retention_days     INT UNSIGNED NULL,
    enabled            BOOLEAN NOT NULL DEFAULT TRUE
) ENGINE=InnoDB;

CREATE TABLE recordings (
    id               BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    camera_id        BIGINT UNSIGNED NOT NULL,
    storage_id       BIGINT UNSIGNED NOT NULL,
    start_time       DATETIME(3) NOT NULL,
    end_time         DATETIME(3) NULL,
    duration_seconds INT UNSIGNED,
    file_size        BIGINT UNSIGNED,
    trigger_type     ENUM('continuous','motion','event','manual') NOT NULL DEFAULT 'continuous',
    status           ENUM('recording','completed','deleted','corrupted') NOT NULL DEFAULT 'recording',
    is_locked        BOOLEAN NOT NULL DEFAULT FALSE,
    FOREIGN KEY (camera_id)  REFERENCES cameras(id),
    FOREIGN KEY (storage_id) REFERENCES storage_locations(id),
    CONSTRAINT chk_recordings_time CHECK (end_time IS NULL OR end_time >= start_time),
    INDEX idx_recordings_camera_time (camera_id, start_time),
    INDEX idx_recordings_status (status),
    INDEX idx_recordings_storage (storage_id)
) ENGINE=InnoDB;

CREATE TABLE recording_segments (
    id               BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    recording_id     BIGINT UNSIGNED NOT NULL,
    file_path        VARCHAR(1000) NOT NULL,
    segment_start    DATETIME(3),
    segment_end      DATETIME(3),
    duration_seconds INT UNSIGNED,
    size_bytes       BIGINT UNSIGNED,
    FOREIGN KEY (recording_id) REFERENCES recordings(id) ON DELETE CASCADE,
    INDEX idx_segments_start (segment_start)
) ENGINE=InnoDB;

CREATE TABLE event_types (
    id               BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    name             VARCHAR(100) NOT NULL UNIQUE,
    description      TEXT,
    default_severity ENUM('low','medium','high','critical') NOT NULL DEFAULT 'low'
) ENGINE=InnoDB;

CREATE TABLE events (
    id               BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    camera_id        BIGINT UNSIGNED NULL,
    event_type_id    BIGINT UNSIGNED NOT NULL,
    event_time       DATETIME(3) NOT NULL,
    severity         ENUM('low','medium','high','critical') NOT NULL DEFAULT 'low',
    title            VARCHAR(255),
    description      TEXT,
    snapshot_path    VARCHAR(1000),
    video_segment_id BIGINT UNSIGNED NULL,
    acknowledged_by  BIGINT UNSIGNED NULL,
    acknowledged_at  DATETIME NULL,
    FOREIGN KEY (camera_id)        REFERENCES cameras(id)            ON DELETE SET NULL,
    FOREIGN KEY (event_type_id)    REFERENCES event_types(id),
    FOREIGN KEY (video_segment_id) REFERENCES recording_segments(id) ON DELETE SET NULL,
    FOREIGN KEY (acknowledged_by)  REFERENCES users(id)              ON DELETE SET NULL,
    INDEX idx_events_time (event_time),
    INDEX idx_events_camera_time (camera_id, event_time),
    INDEX idx_events_type_time (event_type_id, event_time)
) ENGINE=InnoDB;

CREATE TABLE persons (
    id         BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    name       VARCHAR(255) NOT NULL,
    notes      TEXT,
    is_active  BOOLEAN NOT NULL DEFAULT TRUE,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB;

CREATE TABLE person_faces (
    id            BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    person_id     BIGINT UNSIGNED NOT NULL,
    image_path    VARCHAR(1000) NOT NULL,
    embedding     BLOB NOT NULL,
    model_version VARCHAR(50) NOT NULL,
    created_at    DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (person_id) REFERENCES persons(id) ON DELETE CASCADE
) ENGINE=InnoDB;

CREATE TABLE detections (
    id             BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    event_id       BIGINT UNSIGNED NULL,
    camera_id      BIGINT UNSIGNED NULL,
    detection_type ENUM('face','person','vehicle','animal','object') NOT NULL,
    confidence     DECIMAL(5,4),                       
    detected_at    DATETIME(3) NOT NULL,
    bbox_x         DECIMAL(6,5) NULL,
    bbox_y         DECIMAL(6,5) NULL,
    bbox_w         DECIMAL(6,5) NULL,
    bbox_h         DECIMAL(6,5) NULL,
    track_id       BIGINT UNSIGNED NULL,
    FOREIGN KEY (event_id)  REFERENCES events(id)  ON DELETE CASCADE,
    FOREIGN KEY (camera_id) REFERENCES cameras(id) ON DELETE SET NULL,
    CONSTRAINT chk_detections_conf CHECK (confidence IS NULL OR confidence BETWEEN 0 AND 1),
    INDEX idx_detections_camera_time (camera_id, detected_at),
    INDEX idx_detections_type_time (detection_type, detected_at)
) ENGINE=InnoDB;

CREATE TABLE detection_faces (
    id           BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    detection_id BIGINT UNSIGNED NOT NULL UNIQUE,
    person_id    BIGINT UNSIGNED NULL,
    similarity   DECIMAL(5,4),
    age_estimate TINYINT UNSIGNED NULL,
    gender       ENUM('male','female','unknown') NOT NULL DEFAULT 'unknown',
    FOREIGN KEY (detection_id) REFERENCES detections(id) ON DELETE CASCADE,
    FOREIGN KEY (person_id)    REFERENCES persons(id)    ON DELETE SET NULL,
    INDEX idx_detection_faces_person (person_id)
) ENGINE=InnoDB;

CREATE TABLE detection_objects (
    id           BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    detection_id BIGINT UNSIGNED NOT NULL,
    object_class VARCHAR(100) NOT NULL,
    confidence   DECIMAL(5,4),
    FOREIGN KEY (detection_id) REFERENCES detections(id) ON DELETE CASCADE,
    INDEX idx_detection_objects_class (object_class)
) ENGINE=InnoDB;

CREATE TABLE snapshots (
    id         BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    camera_id  BIGINT UNSIGNED NULL,
    event_id   BIGINT UNSIGNED NULL,
    file_path  VARCHAR(1000) NOT NULL,
    created_at DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
    FOREIGN KEY (camera_id) REFERENCES cameras(id) ON DELETE SET NULL,
    FOREIGN KEY (event_id)  REFERENCES events(id)  ON DELETE SET NULL,
    INDEX idx_snapshots_camera_time (camera_id, created_at)
) ENGINE=InnoDB;

CREATE TABLE alert_rules (
    id               BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    name             VARCHAR(150) NOT NULL,
    event_type_id    BIGINT UNSIGNED NULL,
    camera_group_id  BIGINT UNSIGNED NULL,
    min_severity     ENUM('low','medium','high','critical') NOT NULL DEFAULT 'low',
    cooldown_seconds INT UNSIGNED NOT NULL DEFAULT 30,
    enabled          BOOLEAN NOT NULL DEFAULT TRUE,
    send_email       BOOLEAN NOT NULL DEFAULT FALSE,
    send_telegram    BOOLEAN NOT NULL DEFAULT FALSE,
    webhook_url      VARCHAR(1000),
    FOREIGN KEY (event_type_id)   REFERENCES event_types(id)   ON DELETE SET NULL,
    FOREIGN KEY (camera_group_id) REFERENCES camera_groups(id) ON DELETE SET NULL
) ENGINE=InnoDB;

CREATE TABLE alert_logs (
    id       BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    rule_id  BIGINT UNSIGNED NULL,
    event_id BIGINT UNSIGNED NULL,
    sent_at  DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
    channel  ENUM('email','telegram','webhook') NOT NULL,
    status   ENUM('sent','failed','skipped') NOT NULL,
    response TEXT,
    FOREIGN KEY (rule_id)  REFERENCES alert_rules(id) ON DELETE SET NULL,
    FOREIGN KEY (event_id) REFERENCES events(id)      ON DELETE CASCADE,
    INDEX idx_alert_logs_sent (sent_at)
) ENGINE=InnoDB;

CREATE TABLE audit_logs (
    id          BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    user_id     BIGINT UNSIGNED NULL,
    action      VARCHAR(255) NOT NULL,
    entity_type VARCHAR(100),
    entity_id   BIGINT UNSIGNED,
    details     JSON,
    ip_address  VARCHAR(45),
    created_at  DATETIME(3) NOT NULL DEFAULT CURRENT_TIMESTAMP(3),
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE SET NULL,
    INDEX idx_audit_created (created_at),
    INDEX idx_audit_user_time (user_id, created_at),
    INDEX idx_audit_entity (entity_type, entity_id)
) ENGINE=InnoDB;

CREATE TABLE system_settings (
    id            BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    setting_key   VARCHAR(255) NOT NULL UNIQUE,
    setting_value TEXT,
    is_secret     BOOLEAN NOT NULL DEFAULT FALSE,
    updated_at    DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB;